#ifndef TWOSTAGETRADABOOST_7Q4CFB8O
#define TWOSTAGETRADABOOST_7Q4CFB8O

/*
File: TwoStageTrAdaBoost.h
Author: Samuel Barrett
Description: two stage tradaboost - taken from David Pardoe's thesis
Created:  2012-01-20
Modified: 2012-01-20
*/

#include "TrAdaBoost.h"

class TwoStageTrAdaBoost: public Classifier {
public:
  TwoStageTrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations, unsigned int numFolds, int bestT = -1);
  
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);

  virtual void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void reweightData(unsigned int t);
  virtual double calcError(ClassifierPtr newModel, InstanceSet &data);

  virtual void createFolds(std::vector<InstanceSet> &folds);
  virtual ClassifierPtr createModel(int fold, std::vector<InstanceSet> &folds);

protected:
  BaseLearnerGenerator baseLearner;
  Json::Value baseLearnerOptions;
  InstanceSet sourceData;
  InstanceSet targetData;
  ClassifierPtr model;
  const unsigned int maxBoostingIterations;
  const unsigned int numFolds;
  int savedBestT;
};

#endif /* end of include guard: TWOSTAGETRADABOOST_7Q4CFB8O */
