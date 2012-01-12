#ifndef TRADABOOST_3BBYLV2Y
#define TRADABOOST_3BBYLV2Y

/*
File: TrAdaBoost.h
Author: Samuel Barrett
Description: Boosting algorithm for transfer learning
Created:  2011-12-29
Modified: 2011-12-29
*/

#include "Classifier.h"
#include <boost/function.hpp>
#include <json/json.h>

struct BoostingClassifier {
  boost::shared_ptr<Classifier> classifier;
  double betat;
};

typedef boost::function<ClassifierPtr (const std::vector<Feature> &features, const Json::Value &options)> BaseLearnerGenerator;

class TrAdaBoost: public Classifier {
public:
  TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);

  virtual ~TrAdaBoost();
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);
  void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void resetWeights();
  virtual void reweightDistribution();
  virtual double calcError(BoostingClassifier &c);

protected:
  BaseLearnerGenerator baseLearner;
  Json::Value baseLearnerOptions;
  std::vector<BoostingClassifier> classifiers;
  InstanceSet sourceData;
  InstanceSet targetData;
  std::vector<float> absError;
  unsigned int numBoostingIterations;
  const unsigned int maxBoostingIterations;
};

#endif /* end of include guard: TRADABOOST_3BBYLV2Y */
