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
#include <common/Params.h>

class ModelUpdaterSilver: public ModelUpdater {
public:
#define PARAMS(_) \
  _(bool,useFrequencyCounts,weighted,false) \
  _(bool,addUpdateNoise,addUpdateNoise,false) \
  _(float,updateNoise,updateNoise,0.05)

  Params_STRUCT(PARAMS)
#undef PARAMS

  struct StateInfo {
    StateInfo(unsigned int numModels);
    std::vector<float> counts;
    float total;
  };

  typedef std::map<State_t,boost::shared_ptr<StateInfo> > map_t;

public:
  ModelUpdaterSilver(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, const Params &p);
  void updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs);
  void updateSimulationAction(const Action::Type &action, const State_t &state);

protected:
  unsigned int selectModelInd(const State_t &state);
  std::string generateSpecificDescription();

protected:
  map_t stateInfos;
  unsigned int currentBeliefInd;
  Params p;
};

#endif /* end of include guard: MODELUPDATERSILVER_FRGC8XXJ */
