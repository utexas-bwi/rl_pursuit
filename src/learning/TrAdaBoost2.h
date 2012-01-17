#ifndef TRADABOOST2_Z5DHQFPW
#define TRADABOOST2_Z5DHQFPW

#include "AdaBoost.h"

class TrAdaBoost2: public AdaBoost {
public:
  TrAdaBoost2(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations);

protected:
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);
  virtual void reweightData(double alpha);

protected:
};

#endif /* end of include guard: TRADABOOST2_Z5DHQFPW */
