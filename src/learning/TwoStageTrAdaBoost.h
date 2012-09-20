#ifndef TWOSTAGETRADABOOST_7Q4CFB8O
#define TWOSTAGETRADABOOST_7Q4CFB8O

/*
File: TwoStageTrAdaBoost.h
Author: Samuel Barrett
Description: two stage tradaboost - taken from David Pardoe's thesis
Created:  2012-01-20
Modified: 2012-01-20
*/

#include "SubClassifier.h"
#include <common/Params.h>

class TwoStageTrAdaBoost: public Classifier {
public:
#define PARAMS(_) \
  _(unsigned int,maxBoostingIterations,maxBoostingIterations,10) \
  _(unsigned int,numFolds,folds,5) \
  _(int,savedBestT,bestT,-1) \
  _(bool,evaluateSavedBestT,evaluateBestT,false)

  Params_STRUCT(PARAMS)
#undef PARAMS

public:
  TwoStageTrAdaBoost(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, const Params &p);
  
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);
  virtual void addFixedData(const InstancePtr &instance);
  virtual void clearSourceData();
  virtual float getBestSourceInstanceWeight();

  virtual void outputDescription(std::ostream &out) const;
  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);
  virtual void clearData();
  void convertModelFromWekaToDT();

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void reweightData(unsigned int t);
  virtual double calcError(ClassifierPtr newModel, InstanceSet &data);

  virtual void createFolds(std::vector<InstanceSet> &folds);
  virtual ClassifierPtr createModel(int fold, std::vector<InstanceSet> &folds);

  float evaluateWeighting(unsigned int t, std::vector<InstanceSet> &foldedTargetData);

protected:
  SubClassifierGenerator baseLearner;
  Json::Value baseLearnerOptions;
  InstanceSet sourceData;
  InstanceSet targetData;
  InstanceSet fixedData;
  ClassifierPtr model;
  float bestSourceInstanceWeight;
  Params p;
};

#endif /* end of include guard: TWOSTAGETRADABOOST_7Q4CFB8O */
