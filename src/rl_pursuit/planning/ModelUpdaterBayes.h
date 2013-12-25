#ifndef MODELUPDATERBAYES_S4U00VNJ
#define MODELUPDATERBAYES_S4U00VNJ

/*
File: ModelUpdaterBayes.h
Author: Samuel Barrett
Description: a model updater using bayesian updates, also handles updating by polynomial function of the loss
Created:  2011-09-21
Modified: 2011-10-02
*/

#include "ModelUpdaterDiscrete.h"
#include <rl_pursuit/gtest/gtest_prod.h>
#include <rl_pursuit/common/Params.h>
#include <rl_pursuit/common/Enum.h>

ENUM(ModelUpdateType,
  bayesian,
  polynomial,
  none
)

//SET_FROM_JSON_ENUM(ModelUpdateType)

template<class State, class Action>
class ModelUpdaterBayes: public ModelUpdaterDiscrete<State,Action> {

public:
#define PARAMS(_) \
  _(ModelUpdateType_t,modelUpdateType,update,ModelUpdateType::bayesian) \
  _(bool,allowRemovingModels,allowRemovingModels,true) \
  _(float,MIN_MODEL_PROB,minModelProb,0.0001) \
  _(int,stepsUntilSafetyModel,stepsUntilSafetyModel,-1) \
  _(float,lossEta,lossEta,0.5) \
  _(bool,addUpdateNoise,addUpdateNoise,false) \
  _(float,updateNoise,updateNoise,0.05)

  Params_STRUCT(PARAMS)
#undef PARAMS

protected:
  Params p;

public:
  ModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo<State, Action> > &models, const Params &p);
  void updateRealWorldAction(const State &prevState, const Action &lastAction, const State &currentState);
  void updateSimulationAction(const Action &action, const State &state);

protected:
  unsigned int selectModelInd(const State &state);
  void getNewModelProbs(const State &prevState, Action lastAction, const State &currentState, std::vector<double> &newModelProbs);
  double calculateModelProb(unsigned int modelInd, const State &prevState, Action lastAction, const State &currentState);
  bool allProbsTooLow(const std::vector<double> &newModelProbs);
  void removeLowProbabilityModels();
  std::string generateSpecificDescription();

protected:
  FRIEND_TEST(ModelUpdaterBayesTest,AdvancedTests);
};

#undef MODELUPDATERBAYES_DEBUG_MODELS

#include "ModelUpdaterBayes.h"
#include <math.h>

template<class State, class Action>
ModelUpdaterBayes<State,Action>::ModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo<State,Action> > &models, const ModelUpdaterBayes::Params &p):
  ModelUpdaterDiscrete<State,Action>(rng,models),
  p(p)
{
  assert(p.lossEta < 0.5 + 1e-10);
}

template<class State, class Action>
void ModelUpdaterBayes<State,Action>::updateRealWorldAction(const State &prevState, const Action &lastAction, const State &currentState) {
  // done if we're down to 1 model and not outputting the precision of models
  if (models.size() == 1)
    return;
  // done if we're not doing updates
  if (p.modelUpdateType == ModelUpdateType::none)
    return;

  std::vector<double> newModelProbs(models.size());
  for (unsigned int i = 0; i < models.size(); i++)
    newModelProbs[i] = models[i].prob;

#ifdef MODELUPDATERBAYES_DEBUG_MODELS
  std::cout << "ORIG PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
#endif

  // calculate the new model probabilities
  getNewModelProbs(prevState,lastAction,currentState,newModelProbs);
  // reset the mdp
  //mdp->setState(currentState); // TODO removed in model change, is this okay to leave out?
  // normalize the probabilities
  normalizeProbs(newModelProbs);
  // check if all zero probs
  if (allProbsTooLow(newModelProbs)) {
#ifdef MODELUPDATERBAYES_DEBUG_MODELS
    std::cout << "All model probs too low" << std::endl;
#endif
    return;
  }
  // set our models
  for (unsigned int i = 0; i < models.size(); i++)
    models[i].prob = newModelProbs[i];
  // delete models with very low probabilities
  if (p.addUpdateNoise) {
#ifdef MODELUPDATERBAYES_DEBUG_MODELS
  std::cout << "before noise PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
#endif
    for (unsigned int i = 0; i < models.size(); i++)
      models[i].prob = (1 - p.updateNoise) * models[i].prob + p.updateNoise / models.size();
  } else {
    removeLowProbabilityModels();
  }
#ifdef MODELUPDATERBAYES_DEBUG_MODELS
  std::cout << "NEW PROBS: " << std::endl;
  for (unsigned int i = 0; i < models.size(); i++)
    std::cout << "  " << models[i].description << ": " << models[i].prob << std::endl;
#endif
}

template<class State, class Action>
void ModelUpdaterBayes<State,Action>::updateSimulationAction(const Action &, const State &) {
  // DO NOTHING
}

template<class State, class Action>
unsigned int ModelUpdaterBayes<State,Action>::selectModelInd(const State_t &) {
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

template<class State, class Action>
void ModelUpdaterBayes<State,Action>::getNewModelProbs(const State &prevState, Action lastAction, const State &currentState, std::vector<double> &newModelProbs) {
  double modelProb;
  double loss;
  double eta = p.lossEta; // eta must be <= 0.5
  if (precisionOutputStream.get() != NULL)
    (*precisionOutputStream) << "-----" << std::endl;
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProb = calculateModelProb(i,prevState,lastAction,currentState);
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

template<class State, class Action>
double ModelUpdaterBayes<State,Action>::calculateModelProb(unsigned int modelInd, const State &prevState, Action lastAction, const State &currentState) {
  boost::shared_ptr<WorldMDP> mdp = models[modelInd].mdp->clone();
  //boost::shared_ptr<WorldMDP> mdp = models[modelInd].mdp;
  //(*(models[modelInd].mdp));
  //mdp->setAgents(model);
  std::vector<double> agentProbs;
  double prob = mdp->getOutcomeProb(prevState,lastAction,currentState,agentProbs);
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

template<class State, class Action>
bool ModelUpdaterBayes<State,Action>::allProbsTooLow(const std::vector<double> &newModelProbs) {
  // check for a divide by 0
  for (unsigned int i = 0; i < newModelProbs.size(); i++)
    if (isinf(newModelProbs[i]) || isnan(newModelProbs[i]))
      return true;
  return false;
}

template<class State, class Action>
void ModelUpdaterBayes<State,Action>::removeLowProbabilityModels() {
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

template<class State, class Action>
std::string ModelUpdaterBayes<State,Action>::generateSpecificDescription() {
  return getName(p.modelUpdateType);
}

#endif /* end of include guard: MODELUPDATERBAYES_S4U00VNJ */
