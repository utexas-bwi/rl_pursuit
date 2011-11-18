#ifndef FEATUREEXTRACTOR_XSHC1OD2
#define FEATUREEXTRACTOR_XSHC1OD2

/*
File: FeatureExtractor.h
Author: Samuel Barrett
Description: extracts a set of features of the agents
Created:  2011-10-28
Modified: 2011-10-28
*/

#include <model/Common.h>
#include <common/Point2D.h>
#include "DecisionTree.h"
#include <controller/Agent.h>

class FeatureExtractor {
public:
  FeatureExtractor(const Point2D &dims);
  
  void addFeatureAgent(const std::string &key, const std::string &name);
  void extract(const Observation &obs, Features &features);

protected:

protected:
  const Point2D dims;
  boost::unordered_map<std::string,boost::shared_ptr<Agent> > featureAgents;
};

#endif /* end of include guard: FEATUREEXTRACTOR_XSHC1OD2 */
