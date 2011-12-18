#ifndef MODELUPDATERBAYES_S4U00VNJ
#define MODELUPDATERBAYES_S4U00VNJ

/*
File: ModelUpdaterBayes.h
Author: Samuel Barrett
Description: a model updater using bayesian updates, also handles updating by polynomial function of the loss
Created:  2011-09-21
Modified: 2011-10-02
*/

#include "ModelUpdater.h"
#include <gtest/gtest_prod.h>

enum ModelUpdateType {
  BAYESIAN_UPDATES,
  POLYNOMIAL_WEIGHTS,
  NO_MODEL_UPDATES
};

class ModelUpdaterBayes: public ModelUpdater {
public:
  ModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, ModelUpdateType modelUpdateType);
  void updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs);
  void updateSimulationAction(const Action::Type &action, const State_t &state);

protected:
  unsigned int selectModelInd(const State_t &state);
  void getNewModelProbs(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs, std::vector<double> &newModelProbs);
  double calculateModelProb(unsigned int modelInd, const Observation &prevObs, Action::Type lastAction, const Observation &currentObs);
  bool allProbsTooLow(const std::vector<double> &newModelProbs);
  void removeLowProbabilityModels();
  std::string generateSpecificDescription();

protected:
  ModelUpdateType modelUpdateType;
  static const float MIN_MODEL_PROB;

  FRIEND_TEST(ModelUpdaterBayesTest,AdvancedTests);
};

#endif /* end of include guard: MODELUPDATERBAYES_S4U00VNJ */
