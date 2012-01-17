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
#include <common/Util.h>

AdaBoost::AdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations):
  Classifier(features,caching),
  baseLearner(baseLearner),
  baseLearnerOptions(baseLearnerOptions),
  data(numClasses),
  maxBoostingIterations(maxBoostingIterations),
  classifierStartInd(0),
  targetDataStart(0)
{
  assert(baseLearner);
}

AdaBoost::~AdaBoost() {
}

void AdaBoost::addData(const InstancePtr &instance) {
  data.add(instance);
}

void AdaBoost::trainInternal(bool /*incremental*/) {
  //assert(!incremental); // NOT handled for now
  resetWeights();
  classifiers.clear();
  for (unsigned int t = 0; t < maxBoostingIterations; t++) {
    std::cout << "BOOSTING ITERATION: " << t << std::endl;
    normalizeWeights();

    BoostingClassifier c;
    c.classifier = baseLearner(features,baseLearnerOptions);
    for (unsigned int i = 0; i < data.size(); i++)
      c.classifier->addData(data[i]);

    c.classifier->train(false);
    double eps = calcError(c);
    if (1 - eps <= 1.0 / numClasses) { // not helping
      //std::cout << "SHORT CIRCUITING, not helping: " << t << std::endl;
      break;
    }
    c.alpha = log((1.0 - eps) / eps) + log(numClasses - 1.0); // from SAMME
    classifiers.push_back(c);
    if (eps < 0.0001) {
      //std::cout << "SHORT CIRCUITING, perfect: " << t << std::endl;
      break;
    }
    // reweight data
    reweightData(c.alpha);
  }
}

void AdaBoost::reweightData(double alpha) {
  for (unsigned int i = 0; i < data.size(); i++)
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
    data[i]->weight = 1.0;
  data.weight = data.size();
}

void AdaBoost::normalizeWeights() {
  float origWeight = data.weight;
  data.recalculateWeight();
  float factor = origWeight / data.weight;
  for (unsigned int i = 0; i < data.size(); i++)
    data[i]->weight *= factor;
  data.weight *= factor;
}
  
double AdaBoost::calcError(BoostingClassifier &c) {
  absError.resize(data.size());
  InstancePtr inst;
  Classification temp;
  for (unsigned int i = 0; i < data.size(); i++) {
    inst = data[i];
    c.classifier->classify(inst,temp);
    absError[i] = fabs(1.0 - temp[inst->label]);
  }
  
  // calculate epsilon
  if (targetDataStart < 0)
    return 0.0;
  double weight = 0;
  double eps = 0;
  for (unsigned int i = targetDataStart; i < data.size(); i++) {
    eps += data[i]->weight * absError[i];
    weight += data[i]->weight;
  }
  eps = eps / weight;
  return eps;
}

void AdaBoost::outputDescription(std::ostream &out) const {
  out << "AdaBoost" << std::endl;
  for (unsigned int i = 0; i < classifiers.size(); i++)
    out << *(classifiers[i].classifier) << std::endl;
}
