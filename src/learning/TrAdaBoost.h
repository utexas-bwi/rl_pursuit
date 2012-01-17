#ifndef TRADABOOST_Z5DHQFPW
#define TRADABOOST_Z5DHQFPW

#include "AdaBoost.h"

class TrAdaBoost: public AdaBoost {
public:
  TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);

protected:
  virtual void reweightData(double alpha);

protected:
};

#endif /* end of include guard: TRADABOOST_Z5DHQFPW */
