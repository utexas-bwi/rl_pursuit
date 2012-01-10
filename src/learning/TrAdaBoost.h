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

struct BoostingClassifier {
  boost::shared_ptr<Classifier> classifier;
  double betat;
};

typedef boost::function< boost::shared_ptr<Classifier> (const std::vector<Feature> &features, bool caching)> BaseLearnerGenerator;

class TrAdaBoost: public Classifier {
public:
  TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner);

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
  std::vector<BoostingClassifier> classifiers;
  InstanceSet sourceData;
  InstanceSet targetData;
  std::vector<float> absError;
  static const unsigned int numBoostingIterations;
};

#endif /* end of include guard: TRADABOOST_3BBYLV2Y */
