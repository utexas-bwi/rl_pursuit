/*
File: ModelUpdaterSilver.cpp
Author: Samuel Barrett
Description: a model updater using work from Silver's paper, optionally using frequency counts
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "ModelUpdaterSilver.h"
/*
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
*/
ModelUpdaterSilver::StateInfo::StateInfo(unsigned int numModels):
  counts(numModels,0),
  total(0)
{
}

ModelUpdaterSilver::ModelUpdaterSilver(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, const Params &p):
  ModelUpdater(rng,models),
  p(p)
{
}

void ModelUpdaterSilver::updateRealWorldAction(const Observation&, Action::Type, const Observation&) {
  // DO NOTHING
}

void ModelUpdaterSilver::updateSimulationAction(const Action::Type &, const State_t &state) {
  map_t::iterator it = stateInfos.find(state);
  if (it == stateInfos.end()) {
    // previously unseen state
    boost::shared_ptr<StateInfo> info(new StateInfo(models.size()));
    it = stateInfos.insert(std::pair<State_t,boost::shared_ptr<StateInfo> >(state,info)).first;
  }
  StateInfo &stateInfo = *(it->second);
  if (p.useFrequencyCounts) {
    stateInfo.counts[currentBeliefInd]++;
    stateInfo.total++;
  } else {
    if (stateInfo.counts[currentBeliefInd] < 1e-10) {
      stateInfo.counts[currentBeliefInd]++;
      stateInfo.total++;
    }
  }
}

unsigned int ModelUpdaterSilver::selectModelInd(const State_t &state) {
  map_t::iterator it = stateInfos.find(state);
  if (it == stateInfos.end()) {
    // unseen state, sample from the priors
    currentBeliefInd = rng->randomInt(models.size());
  } else {
    float val = rng->randomFloat();
    float total = 0;
    StateInfo &stateInfo = *(it->second);
    for (unsigned int i = 0; i < stateInfo.counts.size(); i++) {
      float prob = stateInfo.counts[i] / stateInfo.total;
      if (p.addUpdateNoise)
        prob = (1 - p.updateNoise) * prob + p.updateNoise / models.size();
      total += prob;
      if (val < total + 1e-10) {
        currentBeliefInd = i;
        break;
      }
    }
  }
  return currentBeliefInd;
}

std::string ModelUpdaterSilver::generateSpecificDescription() {
  std::string msg = "Silver";
  if (p.useFrequencyCounts)
    msg += " Weighted";
  return msg;
}
