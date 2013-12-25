/*
File: AdaBoost.cpp
Author: Samuel Barrett
Description: AdaBoost algorithm, with support for inheritance
Created:  2012-01-16
Modified: 2012-01-16
*/

#include "AdaBoost.h"
#include <cassert>
#include <iostream>
#include <rl_pursuit/common/Util.h>

#include "LinearSVM.h"

AdaBoost::AdaBoost(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations):
  Classifier(features,caching),
  name("AdaBoost"),
  baseLearner(baseLearner),
  baseLearnerOptions(baseLearnerOptions),
  data(numClasses),
  maxBoostingIterations(maxBoostingIterations),
  classifierStartInd(0),
  verbose(true),
  endSourceData(0),
  errorStartInd(0),
  reweightStartInd(0)
{
  assert(baseLearner);
}

AdaBoost::~AdaBoost() {
}

void AdaBoost::addData(const InstancePtr &instance) {
  data.add(instance);
  initialWeights.push_back(instance->weight);
}

void AdaBoost::trainInternal(bool /*incremental*/) {
  //assert(!incremental); // NOT handled for now
  resetWeights();
  classifiers.clear();
  ClassifierPtr lastClassifier;
  
  for (unsigned int t = 0; t < maxBoostingIterations; t++) {
    if (verbose)
      std::cout << "BOOSTING ITERATION: " << t << std::endl;
    normalizeWeights();

    SubClassifier c;
    if (lastClassifier.get() != NULL)
      c.classifier = ClassifierPtr(lastClassifier->copyWithWeights(data));
    if (c.classifier.get() == NULL) {
      baseLearnerOptions["maxNumInstances"] = data.size();
      c.classifier = baseLearner(features,baseLearnerOptions);
      for (unsigned int i = 0; i < endSourceData; i++)
        c.classifier->addSourceData(data[i]);
      for (unsigned int i = endSourceData; i < data.size(); i++)
        c.classifier->addData(data[i]);
    }

    c.classifier->train(false);
    if (lastClassifier.get() == NULL)
      lastClassifier = c.classifier;
    convertWekaToDT(c);

    double eps = calcError(c);
    if (verbose)
      std::cout << "  EPS: " << eps << std::endl;
    if (1 - eps <= 1.0 / numClasses) { // not helping
      if (verbose)
        std::cout << "SHORT CIRCUITING, not helping: " << t << std::endl;
      break;
    }
    c.alpha = log((1.0 - eps) / eps) + log(numClasses - 1.0); // from SAMME
    classifiers.push_back(c);
    if (eps < 0.0001) {
      if (verbose)
        std::cout << "SHORT CIRCUITING, perfect: " << t << std::endl;
      break;
    }
    // reweight data
    reweightData(c.alpha);
  }
}

void AdaBoost::reweightData(double alpha) {
  for (unsigned int i = reweightStartInd; i < data.size(); i++)
    data[i]->weight *= exp(alpha * absError[i]);
}

void AdaBoost::classifyInternal(const InstancePtr &instance, Classification &classification) {
  Classification temp;
  //std::cout << "classify: " << *instance << std::endl;
  for (unsigned int i = classifierStartInd; i < classifiers.size(); i++) {
    classifiers[i].classifier->classify(instance,temp);
    for (unsigned int j = 0; j < numClasses; j++) {
      classification[j] += classifiers[i].alpha * temp[j];
    }
  }
  unsigned int predClass = vectorMaxInd(classification);
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] = 0;
  classification[predClass] = 1.0;
}

void AdaBoost::resetWeights() {
  for (unsigned int i = 0; i < data.size(); i++)
    data[i]->weight = initialWeights[i];
  data.weight = data.size();
}

void AdaBoost::normalizeWeights() {
  float origWeight = data.weight;
  data.recalculateWeight();
  float factor = origWeight / data.weight;
  for (unsigned int i = 0; i < data.size(); i++)
    data[i]->weight *= factor;
  data.weight *= factor;

  if (verbose) {
    float sourceWeight = 0.0;
    float targetWeight = 0.0;
    for (unsigned int i = 0; i < endSourceData; i++)
      sourceWeight += data[i]->weight;
    for (unsigned int i = endSourceData; i < data.size(); i++)
      targetWeight += data[i]->weight;
    std::cout << "Normalizing weights: source: 0 - " << endSourceData << "  target: " << endSourceData << " - " << data.size() << std::endl;
    std::cout << "WEIGHTS: " << sourceWeight << " " << targetWeight << std::endl;
  }
}
  
double AdaBoost::calcError(SubClassifier &c) {
  absError.resize(data.size());
  InstancePtr inst;
  Classification temp;
  for (unsigned int i = 0; i < data.size(); i++) {
    inst = data[i];
    c.classifier->classify(inst,temp);
    absError[i] = fabs(1.0 - temp[inst->label]);
  }
  
  // calculate epsilon
  double weight = 0;
  double eps = 0;
  if (verbose)
    std::cout << "CALCULATING error from " << errorStartInd << " to " << data.size() << std::endl;
  for (unsigned int i = errorStartInd; i < data.size(); i++) {
    eps += data[i]->weight * absError[i];
    weight += data[i]->weight;
  }
  eps = eps / weight;
  return eps;
}

void AdaBoost::outputDescription(std::ostream &out) const {
  out << name << std::endl;
  for (unsigned int i = 0; i < classifiers.size(); i++)
    out << *(classifiers[i].classifier) << std::endl;
}
  
void AdaBoost::save(const std::string &filename) const {
  saveSubClassifiers(classifiers,filename,getSubFilenames(filename,classifiers.size()));
}

bool AdaBoost::load(const std::string &filename) {
  return createAndLoadSubClassifiers(classifiers,filename,features,baseLearner,baseLearnerOptions);
}
