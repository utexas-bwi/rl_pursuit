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

const unsigned int TrAdaBoost::numBoostingIterations = 2; // TODO

TrAdaBoost::TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner):
  Classifier(features,caching),
  baseLearner(baseLearner),
  sourceData(numClasses),
  targetData(numClasses)
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
  for (unsigned int t = 0; t < numBoostingIterations; t++) {
    std::cout << "TRAINING ITERATION: " << t << std::endl;
    reweightDistribution();

    BoostingClassifier c;
    //c.classifier = baseLearner(features,caching);
    c.classifier = baseLearner(features,false); // TODO disabling caching
    for (unsigned int i = 0; i < sourceData.size(); i++)
      c.classifier->addData(sourceData[i]);
    for (unsigned int i = 0; i < targetData.size(); i++)
      c.classifier->addData(targetData[i]);
    c.classifier->train(false);
    double eps = calcError(c);
    double betat = eps / (1 - eps);
    double beta = 1.0 / (1.0 + sqrt(2 * log((double)sourceData.size() / numBoostingIterations)));
    for (unsigned int i = 0; i < sourceData.size(); i++)
      sourceData[i]->weight *= pow(beta,absError[i]);
    for (unsigned int i = 0; i < targetData.size(); i++)
      targetData[i]->weight *= pow(betat,-1 * absError[i + sourceData.size()]);
    c.betat = betat;
    classifiers.push_back(c);
  }
}

void TrAdaBoost::classifyInternal(const InstancePtr &instance, Classification &classification) {
  Classification temp;
  float val;
  std::cout << "classify: " << *instance << std::endl;
  unsigned int startInd = (unsigned int)(numBoostingIterations / 2.0 + 0.5);
  std::cout << "startInd: " << startInd << " " << classifiers.size() << std::endl;
  for (unsigned int i = startInd; i < classifiers.size(); i++) { // TODO TrAdaBoost says start at ceil(n/2) why?
    classifiers[i].classifier->classify(instance,temp);
    std::cout << "  classifier " << i << ":";
    for (unsigned int j = 0; j < numClasses; j++) {
      std::cout << " " << temp[j] << "(" << classifiers[i].betat << "^" << -1 * temp[j] << ")";
      val = pow(classifiers[i].betat,-1 * temp[j]) - pow(classifiers[i].betat,-0.5);
      classification[j] += val;
    }
    std::cout << std::endl;
  }
  float maxVal = -1;
  int maxInd = -1;
  for (unsigned int i = 0; i < numClasses; i++) {
    if (classification[i] > maxVal) {
      maxVal = classification[i];
      maxInd = (int)i;
    }
    classification[i] = 0;
  }
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
  double factor;
  if (targetData.weight > 0.1)
    factor = 1.0 / targetData.weight;
  else
    factor = 1.0;
  double eps = 0;
  for (unsigned int i = 0; i < sourceData.size() + targetData.size(); i++) {
    if (i < sourceData.size())
      inst = sourceData[i];
    else
      inst = targetData[i-sourceData.size()];
    c.classifier->classify(inst,temp);
    absError[i] = fabs(1.0 - temp[inst->label]);
    eps += inst->weight * absError[i] * factor;
  }
  std::cout << "eps: " << eps << std::endl;
  return eps;
}

void TrAdaBoost::outputDescription(std::ostream &out) const {
  out << "TRADABOOST" << std::endl; // TODO
}
