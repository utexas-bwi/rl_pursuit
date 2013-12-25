#ifndef MODELUPDATER_H_NLXO9DYK
#define MODELUPDATER_H_NLXO9DYK

/*
File:     ModelUpdater.h
Author:   Samuel Barrett
Created:  2013-08-08
Modified: 2013-08-08
Description: abstract class for a model updater - selects a model for the MCTS rollouts
*/

#include <boost/shared_ptr.hpp>
#include "Model.h"

template<class State, class Action>
class ModelUpdater {
public:
  typedef boost::shared_ptr<ModelUpdater<State,Action> > Ptr;

  ModelUpdater() {}
  virtual ~ModelUpdater() {}

  virtual boost::shared_ptr<Model<State,Action> > selectModel(const State &state) = 0;
  virtual void updateSimulationAction(const Action &action, const State &state) = 0;
  virtual void updateRealWorldAction(const State &prevState, const Action &lastAction, const State &currentState) = 0;
};

#endif /* end of include guard: MODELUPDATER_H_NLXO9DYK */

