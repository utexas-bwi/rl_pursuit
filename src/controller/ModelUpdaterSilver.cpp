/*
File: ModelUpdaterSilver.cpp
Author: Samuel Barrett
Description: a model updater using work from Silver's paper, optionally using frequency counts
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "ModelUpdaterSilver.h"

StateHelperSilver::StateHelperSilver(bool useFrequencyCounts):
  useFrequencyCounts(useFrequencyCounts)
{
  if (useFrequencyCounts)
    beliefCounts = boost::shared_ptr<std::vector<unsigned int> >(new std::vector<unsigned int>);
}

unsigned int StateHelperSilver::sampleBeliefs(boost::shared_ptr<RNG> rng) {
  if (useFrequencyCounts) {
    float val = rng->randomFloat();
    float total = 0;
    for (unsigned int i = 0; i < beliefs.size(); i++) {
      total += (*beliefCounts)[i] / (float)totalCounts;
      if (val < total)
        return beliefs[i];
    }
    return beliefs.back();
  } else {
    unsigned int ind = rng->randomInt(beliefs.size());
    return beliefs[ind];
  }
}

void StateHelperSilver::addBelief(unsigned int ind) {
  for (unsigned int i = 0; i < beliefs.size(); i++) {
    if (ind == beliefs[i]) {
      if (useFrequencyCounts) {
        (*beliefCounts)[i]++;
        totalCounts++;
      }
      return;
    }
  }
  beliefs.push_back(ind);
  if (useFrequencyCounts) {
    beliefCounts->push_back(1);
    totalCounts++;
  }
}

ModelUpdaterSilver::ModelUpdaterSilver(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions, bool useFrequencyCounts):
  ModelUpdater(rng,mdp,models,modelPrior,modelDescriptions),
  useFrequencyCounts(useFrequencyCounts)
{
}

void ModelUpdaterSilver::updateRealWorldAction(const Observation&, Action::Type, const Observation&) {
  // DO NOTHING
}

void ModelUpdaterSilver::updateSimulationAction(const Action::Type &, const State_t &state) {
  std::map<State_t,boost::shared_ptr<StateHelperSilver> >::iterator it = stateHelpers.find(state);
  if (it == stateHelpers.end()) {
    // previously unseen state
    boost::shared_ptr<StateHelperSilver> helper(new StateHelperSilver(useFrequencyCounts));
    helper->addBelief(currentBeliefInd);
    stateHelpers.insert(std::pair<State_t,boost::shared_ptr<StateHelperSilver> >(state,helper));
  } else {
    // seen state, add this controller as having reached it
    it->second->addBelief(currentBeliefInd);
  }
}

unsigned int ModelUpdaterSilver::selectModelInd(const State_t &state) {
  std::map<State_t,boost::shared_ptr<StateHelperSilver> >::iterator it = stateHelpers.find(state);
  if (it == stateHelpers.end()) {
    // unseen state, sample from the priors
    currentBeliefInd = rng->randomInt(modelProbs.size());
  } else {
    // sample from the state helper
    currentBeliefInd = it->second->sampleBeliefs(rng);
  }
  return currentBeliefInd;
}
