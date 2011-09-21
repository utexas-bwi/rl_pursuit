#ifndef MODELUPDATERSILVER_FRGC8XXJ
#define MODELUPDATERSILVER_FRGC8XXJ

/*
File: ModelUpdaterSilver.h
Author: Samuel Barrett
Description: a model updater using work from Silver's paper, optionally using frequency counts
Created:  2011-09-21
Modified: 2011-09-21
*/

#include <boost/shared_ptr.hpp>
#include "ModelUpdater.h"

class StateHelperSilver {
public:
  StateHelperSilver(bool useFrequencyCounts);
  unsigned int sampleBeliefs(boost::shared_ptr<RNG> rng);
  void addBelief(unsigned int ind);

private:
  bool useFrequencyCounts;
  std::vector<unsigned int> beliefs;
  boost::shared_ptr<std::vector<unsigned int> > beliefCounts;
  unsigned int totalCounts;
};

class ModelUpdaterSilver: public ModelUpdater {
public:
  ModelUpdaterSilver(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions, bool useFrequencyCounts);
  void updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs);
  void updateSimulationAction(const Action::Type &action, const State_t &state);

protected:
  unsigned int selectModelInd(const State_t &state);

protected:
  bool useFrequencyCounts;
  std::map<State_t,boost::shared_ptr<StateHelperSilver> > stateHelpers;
  unsigned int currentBeliefInd;
};

#endif /* end of include guard: MODELUPDATERSILVER_FRGC8XXJ */
