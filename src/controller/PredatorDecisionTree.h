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
#include <learning/Classifier.h>
#include <learning/FeatureExtractor.h>

class PredatorDecisionTree: public Agent {
public:
  PredatorDecisionTree(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<Classifier> classifier, const std::string &name);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  
  PredatorDecisionTree* clone() {
    return new PredatorDecisionTree(*this);
  }


protected:
  const std::string name;
  boost::shared_ptr<Classifier> classifier;
  FeatureExtractor featureExtractor;
};

#endif /* end of include guard: PREDATORDECISIONTREE_L0Z40SLR */
