/*
File: TrAdaBoost.cpp
Author: Samuel Barrett
Description: Boosting algorithm for transfer learning
Created:  2011-12-29
Modified: 2011-12-29
*/

#include "TrAdaBoost.h"
#include <cassert>
#include <iostream>

TrAdaBoost::TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations):
  Classifier(features,caching),
  baseLearner(baseLearner),
  baseLearnerOptions(baseLearnerOptions),
  sourceData(numClasses),
  targetData(numClasses),
  maxBoostingIterations(maxBoostingIterations)
{
  assert(baseLearner);
}

TrAdaBoost::~TrAdaBoost() {
}

void TrAdaBoost::addData(const InstancePtr &instance) {
  targetData.add(instance);
}

void TrAdaBoost::addSourceData(const InstancePtr &instance) {
  sourceData.add(instance);
}

void TrAdaBoost::trainInternal(bool incremental) {
  assert(!incremental); // NOT handled for now
  resetWeights();
  classifiers.clear();
  numBoostingIterations = maxBoostingIterations;
  for (unsigned int t = 0; t < maxBoostingIterations; t++) {
    std::cout << "TRAINING ITERATION: " << t << std::endl;
    reweightDistribution();

    BoostingClassifier c;
    //c.classifier = baseLearner(features,caching);
    //c.classifier = baseLearner(features,false); // TODO disabling caching
    c.classifier = baseLearner(features,baseLearnerOptions);
    for (unsigned int i = 0; i < sourceData.size(); i++)
      c.classifier->addData(sourceData[i]);
    for (unsigned int i = 0; i < targetData.size(); i++)
      c.classifier->addData(targetData[i]);
    //std::cout << "SOURCE DATA" << std::endl;
    //for (unsigned int i = 0; i < sourceData.size(); i++)
      //std::cout << "  " << *(sourceData[i]) << std::endl;
    //std::cout << "TARGET DATA" << std::endl;
    //for (unsigned int i = 0; i < targetData.size(); i++)
      //std::cout << "  " << *(targetData[i]) << std::endl;

    c.classifier->train(false);
    //std::cout << *(c.classifier) << std::endl;
    double eps = calcError(c);
    //std::cout << "EPS: " << eps << std::endl;
    //c.betat = eps / (1 - eps);
    c.betat = eps / ((1.0 - eps) * (numClasses - 1)); // from SAMME
    //std::cout << "BETAT: " << c.betat << std::endl;
    assert(sourceData.size() > numBoostingIterations);
    double beta = 1.0 / (1.0 + sqrt(2 * log((double)sourceData.size() / numBoostingIterations)));
    //std::cout << "BETA: " << beta << std::endl;
    if (1 - eps <= 1.0 / numClasses) { // not helping
      //std::cout << "SHORT CIRCUITING, not helping: " << t << std::endl;
      numBoostingIterations = t;
      break;
    }
    classifiers.push_back(c);
    if (eps < 0.0001) {
      //std::cout << "SHORT CIRCUITING, perfect: " << t << std::endl;
      numBoostingIterations = t + 1;
      break;
    }
    // reweight data
    for (unsigned int i = 0; i < sourceData.size(); i++)
      sourceData[i]->weight *= pow(beta,absError[i]);
    for (unsigned int i = 0; i < targetData.size(); i++)
      targetData[i]->weight *= pow(c.betat,-1 * absError[i + sourceData.size()]);
  }
  //std::cout << "POST TRAINING:" << std::endl;
  //for (unsigned int i = 0; i < classifiers.size(); i++)
    //std::cout << *(classifiers[i].classifier) << std::endl;
}

void TrAdaBoost::classifyInternal(const InstancePtr &instance, Classification &classification) {
  Classification temp;
  float val;
  //std::cout << "classify: " << *instance << std::endl;
  unsigned int startInd = (unsigned int)(numBoostingIterations / 2.0 + 0.5);
  //std::cout << "startInd: " << startInd << " " << classifiers.size() << std::endl;
  for (unsigned int i = startInd; i < classifiers.size(); i++) { // TODO TrAdaBoost says start at ceil(n/2) why?
    classifiers[i].classifier->classify(instance,temp);
    double betat = classifiers[i].betat;
    if (betat < 0.001) {
      if (i == startInd)
        betat = 0.001;
      else
        break;
    }
    //std::cout << "  classifier " << i << ":";
    for (unsigned int j = 0; j < numClasses; j++) {
      //std::cout << " " << temp[j] << "(" << betat << "^" << -1 * temp[j] << ")";
      val = pow(betat,-1 * temp[j]) - pow(betat,-0.5);
      classification[j] += val;
    }
    //std::cout << std::endl;
  }
  float maxVal = -1 * std::numeric_limits<float>::infinity();
  int maxInd = -1;
  //std::cout << "classification: ";
  for (unsigned int i = 0; i < numClasses; i++) {
    //std::cout << classification[i] << " ";
    if (classification[i] > maxVal) {
      maxVal = classification[i];
      maxInd = (int)i;
    }
    classification[i] = 0;
  }
  //std::cout << std::endl;
  //std::cout << "max ind: " << maxInd << std::endl << std::flush;
  assert(maxInd >= 0);
  classification[maxInd] = 1.0;
}

void TrAdaBoost::resetWeights() {
  for (unsigned int i = 0; i < sourceData.size(); i++)
    sourceData[i]->weight = 1.0;
  sourceData.weight = sourceData.size();
  for (unsigned int i = 0; i < targetData.size(); i++)
    targetData[i]->weight = 1.0;
  targetData.weight = targetData.size();
}

void TrAdaBoost::reweightDistribution() {
  sourceData.recalculateWeight();
  targetData.recalculateWeight();
  float totalWeight = sourceData.weight + targetData.weight;
  float factor = 1.0 / totalWeight;
  for (unsigned int i = 0; i < sourceData.size(); i++)
    sourceData[i]->weight *= factor;
  sourceData.weight *= factor;
  for (unsigned int i = 0; i < targetData.size(); i++)
    targetData[i]->weight *= factor;
  targetData.weight *= factor;
}
  
double TrAdaBoost::calcError(BoostingClassifier &c) {
  absError.resize(sourceData.size() + targetData.size());
  InstancePtr inst;
  Classification temp;
  for (unsigned int i = 0; i < sourceData.size() + targetData.size(); i++) {
    if (i < sourceData.size())
      inst = sourceData[i];
    else
      inst = targetData[i-sourceData.size()];
    c.classifier->classify(inst,temp);
    absError[i] = fabs(1.0 - temp[inst->label]);
    //std::cout << i << " " << absError[i] << std::endl;
    //std::cout << *(c.classifier) << std::endl;
    //std::cout << inst->label << " " << inst->weight << " " << absError[i] << " " << temp[inst->label] << std::endl;
  }
  if (targetData.weight < 0.00001)
    return 0;
  // calculate epsilon
  double eps = 0;
  for (unsigned int i = 0; i < targetData.size(); i++)
    eps += targetData[i]->weight * absError[sourceData.size()+i];
  eps = eps / targetData.weight;
  return eps;
}

void TrAdaBoost::outputDescription(std::ostream &out) const {
  out << "TRADABOOST" << std::endl; // TODO
  for (unsigned int i = 0; i < classifiers.size(); i++)
    out << *(classifiers[i].classifier) << std::endl;
}
