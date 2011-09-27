/*
File: ModelUpdaterBayes.cpp
Author: Samuel Barrett
Description: a model updater using bayesian updates, also handles updating by polynomial function of the loss
Created:  2011-09-21
Modified: 2011-09-21
*/

//#define DEBUG_MODELS

#include "ModelUpdaterBayes.h"

const float ModelUpdaterBayes::MIN_MODEL_PROB = 0.001;

ModelUpdaterBayes::ModelUpdaterBayes(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions, ModelUpdateType modelUpdateType):
  ModelUpdater(rng,mdp,models,modelPrior,modelDescriptions),
  modelUpdateType(modelUpdateType)
{
  normalizeModelProbs(modelProbs);
}

void ModelUpdaterBayes::updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  // done if we're down to 1 model
  if (modelProbs.size() == 1)
    return;
  std::vector<double> newModelProbs(modelProbs);

#ifdef DEBUG_MODELS
  std::cout << "ORIG PROBS: " << std::endl;
  for (unsigned int i = 0; i < modelDescriptions.size(); i++)
    std::cout << "  " << modelDescriptions[i] << ": " << modelProbs[i] << std::endl;
#endif

  // calculate the new model probabilities
  getNewModelProbs(prevObs,lastAction,currentObs,newModelProbs);
  // check if all zero probs
  if (allProbsTooLow(newModelProbs)) {
#ifdef DEBUG_MODELS
    std::cout << "All model probs too low" << std::endl;
#endif
    return;
  }
  // set our models
  modelProbs.swap(newModelProbs);
  // normalize the probabilities
  normalizeModelProbs(modelProbs);
  // delete models with very low probabilities
  removeLowProbabilityModels();
#ifdef DEBUG_MODELS
  std::cout << "NEW PROBS: " << std::endl;
  for (unsigned int i = 0; i < modelDescriptions.size(); i++)
    std::cout << "  " << modelDescriptions[i] << ": " << modelProbs[i] << std::endl;
#endif
}

void ModelUpdaterBayes::updateSimulationAction(const Action::Type &, const State_t &) {
  // DO NOTHING
}

unsigned int ModelUpdaterBayes::selectModelInd(const State_t &) {
  // sample the current probs
  float val = rng->randomFloat();
  double total = 0;
  unsigned int selectedInd;
  for (selectedInd = 0; selectedInd < modelProbs.size(); selectedInd++) {
    total += modelProbs[selectedInd];
    if (val < total)
      break;
  }
  return selectedInd;
}

void ModelUpdaterBayes::getNewModelProbs(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs, std::vector<double> &newModelProbs) {
  double modelProb;
  double loss;
  double eta = 0.5; // eta must be <= 0.5
  for (unsigned int i = 0; i < modelProbs.size(); i++) {
    modelProb = calculateModelProb(i,prevObs,lastAction,currentObs);
    switch(modelUpdateType) {
      case BAYESIAN_UPDATES:
        newModelProbs[i] *= modelProb;
        break;
      case POLYNOMIAL_WEIGHTS:
        // TODO Look at this again
        loss = 1.0 - modelProb;
        newModelProbs[i] *= (1 - eta * loss);
        break;
    }
  }
}

double ModelUpdaterBayes::calculateModelProb(unsigned int modelInd, const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  mdp->setAgents(models[modelInd]);
  double prob = mdp->getOutcomeProb(prevObs,lastAction,currentObs);
  //std::cout << "CALCULATE MODEL PROB FOR " << modelInd << " = " << prob << std::endl;
  return prob;
}

bool ModelUpdaterBayes::allProbsTooLow(const std::vector<double> &newModelProbs) {
  for (unsigned int i = 0; i < newModelProbs.size(); i++)
    if (newModelProbs[i] >= MIN_MODEL_PROB)
      return false;
  return true;
}

void ModelUpdaterBayes::removeLowProbabilityModels() {
  bool removedModels = false;
  unsigned int i = 0;
  // remove the models that are below the threshold
  while (i < modelProbs.size()) {
    if (modelProbs[i] < MIN_MODEL_PROB) {
      removeModel(i);
      removedModels = true;
    } else {
      ++i;
    }
  }
  // renormalize if we removed models
  if (removedModels)
    normalizeModelProbs(modelProbs);
}

std::string ModelUpdaterBayes::generateSpecificDescription() {
  switch (modelUpdateType) {
    case BAYESIAN_UPDATES:
      return "Bayesian";
      break;
    case POLYNOMIAL_WEIGHTS:
      return "Polynomial";
      break;
  }
}
