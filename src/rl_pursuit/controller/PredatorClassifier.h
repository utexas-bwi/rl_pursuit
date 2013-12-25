#ifndef PREDATORDECISIONTREE_L0Z40SLR
#define PREDATORDECISIONTREE_L0Z40SLR

/*
File: PredatorClassifier.h
Author: Samuel Barrett
Description: a predator that selects actions using a decision tree
Created:  2011-09-15
Modified: 2011-12-02
*/

#include "Agent.h"
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <rl_pursuit/learning/Classifier.h>
#include <rl_pursuit/learning/FeatureExtractor.h>

class PredatorClassifier: public Agent {
public:
  PredatorClassifier(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<Classifier> classifier, const std::string &name, int trainingPeriod, bool trainIncremental);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  void learn(const Observation &prevObs, const Observation &currentObs, unsigned int ind);
  //void minimalStep(const Observation &obs);
  
  PredatorClassifier* clone() {
    return new PredatorClassifier(*this);
  }

  boost::shared_ptr<Classifier> getClassifier() {
    return classifier;
  }

  void setPreventTraining(bool inPreventTraining) {
    preventTraining = inPreventTraining;
  }

protected:
  const std::string name;
  boost::shared_ptr<Classifier> classifier;
  FeatureExtractor featureExtractor;
  FeatureExtractorHistory stepHistory;
  FeatureExtractorHistory learnHistory;
  int trainingPeriod;
  int trainingCounter;
  bool trainIncremental;
  bool preventTraining;
};

#endif /* end of include guard: PREDATORDECISIONTREE_L0Z40SLR */
