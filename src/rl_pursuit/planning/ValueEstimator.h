#ifndef VALUEESTIMATOR_VKVQ0GPY
#define VALUEESTIMATOR_VKVQ0GPY

/*
File: ValueEstimator.h
Author: Samuel Barrett
Description: an abstract value estimator used for planning
Created:  2011-08-23
Modified: 2013-08-08
*/

#include <string>
#include <boost/shared_ptr.hpp>
#include "Model.h"

template<class State, class Action>
class ValueEstimator {
public:
  typedef boost::shared_ptr<ValueEstimator<State,Action> > Ptr;

  ValueEstimator () {}

  virtual Action selectWorldAction(const State &state) = 0;
  virtual Action selectPlanningAction(const State &state) = 0;
  virtual void startRollout() = 0;
  virtual void finishRollout(const State &state, bool terminal) = 0;
  virtual void visit(const State &state, const Action &action, float reward) = 0;
  virtual void restart() = 0;
  virtual std::string generateDescription(unsigned int indentation = 0) = 0;
  virtual void pruneOldVisits(int memorySize) = 0; // 1 keeps the most recent, 0 keeps none, <0 means no pruning

  virtual void setModel(boost::shared_ptr<Model<State,Action> > nmodel) {
    model = nmodel;
  }
protected:
  boost::shared_ptr<Model<State,Action> > model;
};

#endif /* end of include guard: VALUEESTIMATOR_VKVQ0GPY */
