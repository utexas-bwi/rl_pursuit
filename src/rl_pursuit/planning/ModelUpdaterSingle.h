#ifndef MODELUPDATERSINGLE_H_4U0PGTNV
#define MODELUPDATERSINGLE_H_4U0PGTNV

/*
File:     ModelUpdaterSingle.h
Author:   Samuel Barrett
Created:  2013-08-08
Modified: 2013-08-08
Description: a single model updater that has a single model
*/

#include "ModelUpdater.h"

template<class State, class Action>
class ModelUpdaterSingle: public ModelUpdater<State,Action> {
public:
  typedef ModelUpdaterSingle<State,Action> Ptr;

  ModelUpdaterSingle(boost::shared_ptr<Model<State,Action> > model):
    model(model)
  {
  }
  virtual ~ModelUpdaterSingle() {}
  
  virtual boost::shared_ptr<Model<State,Action> > selectModel(const State &state) {
    model->setState(state);
    return model;
  }

  virtual void updateSimulationAction(const Action &/*action*/, const State &/*state*/) {
  }

  virtual void updateRealWorldAction(const State &/*prevState*/, const Action &/*lastAction*/, const State &/*currentState*/) {
  }

protected:
  boost::shared_ptr<Model<State,Action> > model;
};

#endif /* end of include guard: MODELUPDATERSINGLE_H_4U0PGTNV */

