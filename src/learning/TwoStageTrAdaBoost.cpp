/*
File: TwoStageTrAdaBoost.cpp
Author: Samuel Barrett
Description: two stage tradaboost - taken from David Pardoe's thesis
Created:  2012-01-20
Modified: 2012-01-20
*/

#include "TwoStageTrAdaBoost.h"
  
TwoStageTrAdaBoost::TwoStageTrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations, unsigned int numFolds, int bestT):
  Classifier(features,caching),
  baseLearner(baseLearner),
  baseLearnerOptions(baseLearnerOptions),
  sourceData(numClasses),
  targetData(numClasses),
  maxBoostingIterations(maxBoostingIterations),
  numFolds(numFolds),
  savedBestT(bestT)
{
  assert(baseLearner);
}

void TwoStageTrAdaBoost::addData(const InstancePtr &instance) {
  targetData.add(instance);
}

void TwoStageTrAdaBoost::addSourceData(const InstancePtr &instance) {
  sourceData.add(instance);
}
  
void TwoStageTrAdaBoost::outputDescription(std::ostream &out) const {
  out << "TwoStageTrAdaBoost" << std::endl;
  out << *model << std::endl;
}

void TwoStageTrAdaBoost::trainInternal(bool /*incremental*/) {
  assert(targetData.size() > 0); // doesn't make sense otherwise
  std::vector<InstanceSet> foldedTargetData(numFolds,InstanceSet(numClasses));
  createFolds(foldedTargetData);
  
  unsigned int bestT = 0;
  if (savedBestT <= 0) {
    ClassifierPtr newModel;
    float bestError = std::numeric_limits<float>::infinity();
    for (unsigned int t = 1; t <= maxBoostingIterations; t++) {
      reweightData(t);
      float error = 0.0;
      for (unsigned int fold = 0; fold < numFolds; fold++) {
        newModel = createModel(fold,foldedTargetData);
        error += calcError(newModel,foldedTargetData[fold]) / numFolds;
      }
      //std::cout << "error: " << error << std::endl;
      if (error < bestError) {
        bestError = error;
        bestT = t;
        //std::cout << "NEW BEST: " << t << std::endl;
      }
    }
  } else {
    bestT = savedBestT;
    std::cout << "WARNING: TwoStageTrAdaBoost isn't figuring out the best weighting, just using the one provided" << std::endl;
  }
  // make the real model for the best t
  std::cout << "BEST T: " << bestT << std::endl;
  reweightData(bestT);
  model = createModel(-1,foldedTargetData);
}

void TwoStageTrAdaBoost::classifyInternal(const InstancePtr &instance, Classification &classification) {
  assert(model.get() != NULL);
  model->classify(instance,classification);
}

void TwoStageTrAdaBoost::reweightData(unsigned int t) {
  float n = sourceData.size();
  float m = targetData.size();
  float fracTargetWeight = m / (n + m) + (t / ((float)maxBoostingIterations)) * (1 - m / (n + m));
  float fracSourceWeight = 1 - fracTargetWeight;
  // per instance
  float totalWeight = m / fracTargetWeight;
  float targetWeight = fracTargetWeight * totalWeight / m;
  float sourceWeight = (fracSourceWeight * totalWeight) / n;
  //std::cout << "TARGET WEIGHT: " << targetWeight << "  SOURCE WEIGHT: " << sourceWeight << std::endl;
  std::cout << "totalTarget: " << m * targetWeight << "  totalSource: " << n * sourceWeight << std::endl;
  for (unsigned int i = 0; i < sourceData.size(); i++)
    sourceData[i]->weight = sourceWeight;
  for (unsigned int i = 0; i < targetData.size(); i++)
    targetData[i]->weight = targetWeight;
}

double TwoStageTrAdaBoost::calcError(ClassifierPtr newModel, InstanceSet &data) {
  double error = 0.0;
  Classification temp;
  for (unsigned int i = 0; i < data.size(); i++) {
    newModel->classify(data[i],temp);
    error += 1.0 - temp[data[i]->label];
  }

  return error;
}
  
void TwoStageTrAdaBoost::createFolds(std::vector<InstanceSet> &folds) {
  for (unsigned int i = 0; i < targetData.size(); i++) {
    int fold = rng->randomInt(numFolds);
    folds[fold].add(targetData[i]);
  }
}

ClassifierPtr TwoStageTrAdaBoost::createModel(int fold, std::vector<InstanceSet> &folds) {
  ClassifierPtr newModel = baseLearner(features,baseLearnerOptions);
  for (unsigned int i = 0; i < sourceData.size(); i++)
    newModel->addSourceData(sourceData[i]);
  for (unsigned int i = 0; i < numFolds; i++) {
    if ((int)i == fold)
      continue;
    for (unsigned int j = 0; j < folds[i].size(); j++)
      newModel->addData(folds[i][j]);
  }
  newModel->train(false);
  return newModel;
}
