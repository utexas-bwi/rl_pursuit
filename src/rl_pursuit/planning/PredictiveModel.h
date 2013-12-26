#ifndef PREDICTIVEMODEL_KNIP16QL
#define PREDICTIVEMODEL_KNIP16QL

/*
File: PredictiveModel.h
Author: Piyush Khandelwal
Description: an abstract model for planning requiring predictive models 
Created:  2013-07-23
*/

#include <string>
#include <vector>

template<class State, class Action>
class PredictiveModel {
public:
  PredictiveModel () {}
  virtual ~PredictiveModel () {}

  virtual bool isTerminalState(const State &state) const = 0;
  virtual void getActionsAtState(const State &state, std::vector<Action>& actions) = 0;
  virtual void getStateVector(std::vector<State>& states) = 0; 
  virtual void getTransitionDynamics(const State &state, 
      const Action &action, std::vector<State> &next_states, 
      std::vector<float> &rewards, std::vector<float> &probabilities) = 0;

  virtual std::string generateDescription(unsigned int indentation = 0) = 0;
};

#endif /* end of include guard: PREDICTIVEMODEL_KNIP16QL */
