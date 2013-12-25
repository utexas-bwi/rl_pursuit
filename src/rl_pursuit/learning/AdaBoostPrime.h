#ifndef ADABOOSTPRIME_FE3XTIGL
#define ADABOOSTPRIME_FE3XTIGL

/*
File: AdaBoostPrime.h
Author: Samuel Barrett
Description: AdaBoost' from David Pardoe's thesis: AdaBoost that doesn't change the source data weights
Created:  2012-01-20
Modified: 2012-01-20
*/

#include "AdaBoost.h"

class AdaBoostPrime: public AdaBoost {
public:
  AdaBoostPrime(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);
protected:
  bool onlySourceData;
};

#endif /* end of include guard: ADABOOSTPRIME_FE3XTIGL */
