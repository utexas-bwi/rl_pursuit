#ifndef VALUEESTIMATOR_VKVQ0GPY
#define VALUEESTIMATOR_VKVQ0GPY

/*
File: ValueEstimator.h
Author: Samuel Barrett
Description: an abstract value estimator used for planning
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <string>

template<class State, class Action>
class ValueEstimator {
public:
  ValueEstimator () {}

  virtual Action selectWorldAction(const State &state) = 0;
  virtual Action selectPlanningAction(const State &state) = 0;
  virtual void startRollout(const State &state) = 0;
  virtual void finishRollout(bool terminal) = 0;
  virtual void visit(const Action &action, float reward, const State &state) = 0;
  virtual void restart() = 0;
  virtual std::string generateDescription(unsigned int indentation = 0) = 0;
};

#endif /* end of include guard: VALUEESTIMATOR_VKVQ0GPY */
