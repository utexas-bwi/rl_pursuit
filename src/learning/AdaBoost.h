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
#include <boost/function.hpp>
#include <json/json.h>

struct BoostingClassifier {
  boost::shared_ptr<Classifier> classifier;
  double alpha;
};

typedef boost::function<ClassifierPtr (const std::vector<Feature> &features, const Json::Value &options)> BaseLearnerGenerator;

class AdaBoost: public Classifier {
public:
  AdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);

  virtual ~AdaBoost();
  virtual void addData(const InstancePtr &instance);
  void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void resetWeights();
  virtual void normalizeWeights();
  virtual void reweightData(double alpha);
  virtual double calcError(BoostingClassifier &c, unsigned int targetInd);

protected:
  BaseLearnerGenerator baseLearner;
  Json::Value baseLearnerOptions;
  std::vector<BoostingClassifier> classifiers;
  InstanceSet data;
  std::vector<float> absError;
  unsigned int numBoostingIterations;
  const unsigned int maxBoostingIterations;
  unsigned int classifierStartInd;
};

#endif /* end of include guard: ADABOOST_YK4JLNUB */
