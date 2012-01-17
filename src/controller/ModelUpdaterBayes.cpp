/*
File: ModelUpdaterBayes.cpp
Author: Samuel Barrett
Description: a model updater using bayesian updates, also handles updating by polynomial function of the loss
Created:  2011-09-21
Modified: 2011-10-02
*/

#undef DEBUG_MODELS

#include "ModelUpdaterBayes.h"
#include <math.h>

const float ModelUpdaterBayes::MIN_MODEL_PROB = 0.001;

ModelUpdaterBayes::ModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, ModelUpdateType modelUpdateType):
  ModelUpdater(rng,models),
  modelUpdateType(modelUpdateType)
{
}

void ModelUpdaterBayes::updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  //std::cout << "  " << prevObs << " " << lastAction << std::endl;
  //std::cout << "  " << currentObs << std::endl;
  // done if we're down to 1 model
  if (models.size() == 1)
    return;
  // done if we're not doing updates
  if (modelUpdateType == NO_MODEL_UPDATES)
    return;

  std::vector<double> newModelProbs(models.size());
  for (unsigned int i = 0; i < models.size(); i++)
    newModelProbs[i] = models[i].prob;

#ifdef DEBUG_MODELS
  std::cout << "ORIG PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
#endif

  // calculate the new model probabilities
  getNewModelProbs(prevObs,lastAction,currentObs,newModelProbs);
  // reset the mdp
  //mdp->setState(currentObs); // TODO removed in model change, is this okay to leave out?
  // normalize the probabilities
  normalizeProbs(newModelProbs);
  // check if all zero probs
  if (allProbsTooLow(newModelProbs)) {
#ifdef DEBUG_MODELS
    std::cout << "All model probs too low" << std::endl;
#endif
    return;
  }
  // set our models
  for (unsigned int i = 0; i < models.size(); i++)
    models[i].prob = newModelProbs[i];
  // delete models with very low probabilities
  removeLowProbabilityModels();
#ifdef DEBUG_MODELS
  std::cout << "NEW PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
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
  for (selectedInd = 0; selectedInd < models.size() - 1; selectedInd++) { // -1 because if it's not in the first n - 1, it's in the last and it makes the later processing easier
    total += models[selectedInd].prob;
    if (val < total)
      break;
  }
  return selectedInd;
}

void ModelUpdaterBayes::getNewModelProbs(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs, std::vector<double> &newModelProbs) {
  double modelProb;
  double loss;
  double eta = 0.5; // eta must be <= 0.5
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProb = calculateModelProb(i,prevObs,lastAction,currentObs);
    switch(modelUpdateType) {
      case BAYESIAN_UPDATES:
        newModelProbs[i] *= modelProb;
        break;
      case POLYNOMIAL_WEIGHTS:
        loss = 1.0 - modelProb;
        newModelProbs[i] *= (1 - eta * loss);
        break;
      case NO_MODEL_UPDATES:
        assert(false);
        break;
    }
  }
}

double ModelUpdaterBayes::calculateModelProb(unsigned int modelInd, const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  boost::shared_ptr<WorldMDP> mdp = models[modelInd].mdp->clone();
  //boost::shared_ptr<WorldMDP> mdp = models[modelInd].mdp;
  //(*(models[modelInd].mdp));
  //mdp->setAgents(model);
  double prob = mdp->getOutcomeProb(prevObs,lastAction,currentObs);
  //std::cout << "    CALCULATE MODEL PROB FOR " << modelInd << " = " << prob << std::endl;
  return prob;
}

bool ModelUpdaterBayes::allProbsTooLow(const std::vector<double> &newModelProbs) {
  // check for a divide by 0
  for (unsigned int i = 0; i < newModelProbs.size(); i++)
    if (isinf(newModelProbs[i]) || isnan(newModelProbs[i]))
      return true;
  return false;
}

void ModelUpdaterBayes::removeLowProbabilityModels() {
  bool removedModels = false;
  unsigned int i = 0;
  // remove the models that are below the threshold
  while (i < models.size()) {
    if (models[i].prob < MIN_MODEL_PROB) {
      removeModel(i);
      removedModels = true;
    } else {
      ++i;
    }
  }
  // renormalize if we removed models
  if (removedModels)
    normalizeModelProbs();
}

std::string ModelUpdaterBayes::generateSpecificDescription() {
  switch (modelUpdateType) {
    case BAYESIAN_UPDATES:
      return "Bayesian";
      break;
    case POLYNOMIAL_WEIGHTS:
      return "Polynomial";
      break;
    case NO_MODEL_UPDATES:
      return "None";
      break;
    default:
      assert(false);
  }
}
