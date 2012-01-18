#ifndef TRBAGG_LVCVABXW
#define TRBAGG_LVCVABXW

/*
File: TrBagg.h
Author: Samuel Barrett
Description: implementation of the TrBagg algorithm
Created:  2012-01-18
Modified: 2012-01-18
*/

#include "Classifier.h"
#include "AdaBoost.h"
#include <common/RNG.h>

class TrBagg: public Classifier {
public:
  TrBagg(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations, BaseLearnerGenerator fallbackLearner, const Json::Value &fallbackLearnerOptions);
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);

  virtual void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification, const std::vector<BoostingClassifier> &classifiers);

  virtual void calcErrorOfClassifier(BoostingClassifier &c);
  virtual double calcErrorOfSet(unsigned int size);
  virtual double calcErrorOfSet(const std::vector<BoostingClassifier> &classifiers);

  virtual unsigned int selectSize(const std::vector<BoostingClassifier> &classifiers);
  virtual double calcErrorOfSet(unsigned int size, const std::vector<std::vector<Classification> > &classifications);

protected:
  BaseLearnerGenerator baseLearner;
  Json::Value baseLearnerOptions;
  BaseLearnerGenerator fallbackLearner;
  Json::Value fallbackLearnerOptions;
  std::vector<BoostingClassifier> classifiers;
  InstanceSet data;
  const unsigned int maxBoostingIterations;
  int targetDataStart;
};

#endif /* end of include guard: TRBAGG_LVCVABXW */
