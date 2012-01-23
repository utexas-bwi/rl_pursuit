#include "TrAdaBoost.h"

TrAdaBoost::TrAdaBoost(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations):
  AdaBoost(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations),
  onlySourceData(true)
{
  name = "TrAdaBoost";
}

void TrAdaBoost::addData(const InstancePtr &instance) {
  onlySourceData = false;
  AdaBoost::addData(instance);
}

void TrAdaBoost::addSourceData(const InstancePtr &instance) {
  assert(onlySourceData);
  AdaBoost::addData(instance);
  endSourceData = data.size();
  errorStartInd = data.size();
}

void TrAdaBoost::reweightData(double alpha) {
  assert(!onlySourceData);
  // source data
  double N = maxBoostingIterations;
  double n = endSourceData;
  double beta = 1.0 / (1.0 + sqrt((2.0 / N) * log(n)));
  double sourceAlpha = log(beta);
  for (unsigned int i = 0; i < endSourceData; i++)
    data[i]->weight *= exp(sourceAlpha * absError[i]);
  // target data
  for (unsigned int i = endSourceData; i < data.size(); i++)
    data[i]->weight *= exp(alpha * absError[i]);
}
