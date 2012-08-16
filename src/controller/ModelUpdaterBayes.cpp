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

//const float ModelUpdaterBayes::MIN_MODEL_PROB = 0.001;

ModelUpdaterBayes::ModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, const ModelUpdaterBayes::Params &p):
  ModelUpdater(rng,models),
  p(p),
  safetyModel(NULL)
{
  assert(p.lossEta < 0.5 + 1e-10);
  if (p.stepsUntilSafetyModel >= 0) {
    for (unsigned int i = 0; i < models.size(); i++) {
      if (models[i].description == p.safetyModelDesc) {
        safetyModel = new ModelInfo(models[i]);
        safetyModel->description = "SAFETY-" + safetyModel->description;
        std::cout << "FOUND SAFETY MODEL" << std::endl;
        break;
      }
    }
    if (safetyModel == NULL) {
      std::cerr << "Safety Model not found, exitting" << std::endl;
      exit(45);
    }
  }
}

void ModelUpdaterBayes::updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  if (p.stepsUntilSafetyModel == 0) {
    models.clear();
    models.push_back(*safetyModel);
    std::cout << "SWITCHING TO SAFETY" << std::endl;
  }
  p.stepsUntilSafetyModel--;
  
  //std::cout << "  " << prevObs << " " << lastAction << std::endl;
  //std::cout << "  " << currentObs << std::endl;
  // done if we're down to 1 model and not outputting the precision of models
  if ((models.size() == 1) && (precisionOutputStream.get() == NULL))
    return;
  // done if we're not doing updates
  if (p.modelUpdateType == ModelUpdateType::none)
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
  if (p.addUpdateNoise) {
#ifdef DEBUG_MODELS
  std::cout << "before noise PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
#endif
    for (unsigned int i = 0; i < models.size(); i++)
      models[i].prob = (1 - p.updateNoise) * models[i].prob + p.updateNoise / models.size();
  } else {
    removeLowProbabilityModels();
  }
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
  double eta = p.lossEta; // eta must be <= 0.5
  if (precisionOutputStream.get() != NULL)
    (*precisionOutputStream) << "-----" << std::endl;
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProb = calculateModelProb(i,prevObs,lastAction,currentObs);
    switch(p.modelUpdateType) {
      case ModelUpdateType::bayesian:
        newModelProbs[i] *= modelProb;
        break;
      case ModelUpdateType::polynomial:
        loss = 1.0 - modelProb;
        newModelProbs[i] *= (1 - eta * loss);
        break;
      case ModelUpdateType::none:
        assert(false);
        break;
      default:
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
  std::vector<double> agentProbs;
  double prob = mdp->getOutcomeProb(prevObs,lastAction,currentObs,agentProbs);
  if (precisionOutputStream.get() != NULL) {
    std::ostream &out = *precisionOutputStream;
    out << models[modelInd].description;
    for (unsigned int i = 0; i < agentProbs.size(); i++) {
      out << "," << agentProbs[i];
    }
    out << std::endl;
  }
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
    if (models[i].prob < p.MIN_MODEL_PROB) {
      if (p.allowRemovingModels)
        removeModel(i);
      else {
        models[i].prob = p.MIN_MODEL_PROB;
        ++i;
      }
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
  return getName(p.modelUpdateType);
}
