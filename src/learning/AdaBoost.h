#ifndef ADABOOST_YK4JLNUB
#define ADABOOST_YK4JLNUB

/*
File: AdaBoost.h
Author: Samuel Barrett
Description: AdaBoost algorithm, with support for inheritance
Created:  2012-01-16
Modified: 2012-01-16
*/

#include "Classifier.h"
#include "SubClassifier.h"
#include <json/json.h>

class AdaBoost: public Classifier {
public:
  AdaBoost(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);

  virtual ~AdaBoost();
  virtual void addData(const InstancePtr &instance);
  void outputDescription(std::ostream &out) const;
  virtual void setVerbose(bool flag) {
    verbose = flag;
  }
  
  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void resetWeights();
  virtual void normalizeWeights();
  virtual void reweightData(double alpha);
  virtual double calcError(SubClassifier &c);
  virtual void clearData() {
    data.clearData();
  }

  void convertWeka(SubClassifier &c);

protected:
  std::string name;
  SubClassifierGenerator baseLearner;
  Json::Value baseLearnerOptions;
  std::vector<SubClassifier> classifiers;
  InstanceSet data;
  std::vector<float> initialWeights;
  std::vector<float> absError;
  const unsigned int maxBoostingIterations;
  unsigned int classifierStartInd;
  bool verbose;

  unsigned int endSourceData;
  unsigned int errorStartInd;
  unsigned int reweightStartInd;
};

#endif /* end of include guard: ADABOOST_YK4JLNUB */
