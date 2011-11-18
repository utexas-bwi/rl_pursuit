#ifndef PREDATORDECISIONTREE_L0Z40SLR
#define PREDATORDECISIONTREE_L0Z40SLR

/*
File: PredatorDecisionTree.h
Author: Samuel Barrett
Description: a predator that selects actions using a decision tree
Created:  2011-09-15
Modified: 2011-09-15
*/

#include "Agent.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <learning/DecisionTree.h>
#include <learning/FeatureExtractor.h>

class PredatorDecisionTree: public Agent {
public:
  PredatorDecisionTree(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &filename);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  
  PredatorDecisionTree* clone() {
    return new PredatorDecisionTree(*this);
  }


protected:
  const std::string filename;
  boost::shared_ptr<DecisionTree> decisionTree;
  FeatureExtractor featureExtractor;
};

#endif /* end of include guard: PREDATORDECISIONTREE_L0Z40SLR */
