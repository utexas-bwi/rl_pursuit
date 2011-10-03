#ifndef DUALUCTESTIMATOR_F3MKQ7CG
#define DUALUCTESTIMATOR_F3MKQ7CG

/*
File: DualUCTEstimator.h
Author: Samuel Barrett
Description: combines 2 value estimators
Created:  2011-10-01
Modified: 2011-10-01
*/

#include <boost/shared_ptr.hpp>

#include "UCTEstimator.h"

template<class State, class Action>
class DualUCTEstimator: public ValueEstimator<State,Action> {
public:
  DualUCTEstimator(boost::shared_ptr<UCTEstimator<State,Action> > mainValueEstimator, float b);
  void setAssistingValueEstimator(boost::shared_ptr<UCTEstimator<State,Action> > newEstimator);

  Action selectWorldAction(const State &state);
  Action selectPlanningAction(const State &state);
  void startRollout();
  void finishRollout(const State &state, bool terminal);
  void visit(const State &state, const Action &action, float reward);
  void restart();
  std::string generateDescription(unsigned int indentation = 0);
  
  Action selectAction(const State &state, bool useBounds);
  float calcActionValue(const State &state, const Action &action, bool useBounds);

private:
  boost::shared_ptr<UCTEstimator<State,Action> > mainValueEstimator;
  boost::shared_ptr<UCTEstimator<State,Action> > assistingValueEstimator;
  float b;
};

template<class State, class Action>
DualUCTEstimator<State,Action>::DualUCTEstimator(boost::shared_ptr<UCTEstimator<State,Action> > mainValueEstimator, float b):
  mainValueEstimator(mainValueEstimator),
  b(b)
{
}

template<class State, class Action>
void DualUCTEstimator<State,Action>::setAssistingValueEstimator(boost::shared_ptr<UCTEstimator<State,Action> > newEstimator) {
  assistingValueEstimator = newEstimator;
}

template<class State, class Action>
Action DualUCTEstimator<State,Action>::selectWorldAction(const State &state) {
  return selectAction(state,false);
}

template<class State, class Action>
Action DualUCTEstimator<State,Action>::selectPlanningAction(const State &state) {
  return selectAction(state,true);
}

template<class State, class Action>
void DualUCTEstimator<State,Action>::startRollout() {
  mainValueEstimator->startRollout();
  assistingValueEstimator->startRollout();
}

template<class State, class Action>
void DualUCTEstimator<State,Action>::finishRollout(const State &state, bool terminal) {
  mainValueEstimator->finishRollout(state,terminal);
  assistingValueEstimator->finishRollout(state,terminal);
}

template<class State, class Action>
void DualUCTEstimator<State,Action>::visit(const State &state, const Action &action, float reward) {
  mainValueEstimator->visit(state,action,reward);
  assistingValueEstimator->visit(state,action,reward);
}

template<class State, class Action>
void DualUCTEstimator<State,Action>::restart() {
  mainValueEstimator->restart();
  assistingValueEstimator->restart();
}

template<class State, class Action>
std::string DualUCTEstimator<State,Action>::generateDescription(unsigned int indentation) {
  return indent(indentation) + "DualUCTEstimator: combining the value functions:\n" + mainValueEstimator->generateDescription(indentation+1) + "\n" + assistingValueEstimator->generateDescription(indentation+1):
}

template<class State, class Action>
float DualUCTEstimator<State,Action>::calcActionValue(const State &state, const Action &action, bool useBounds) {
  unsigned int n = mainValueEstimator->getNumVisits(state,action);
  unsigned int nh = assistingValueEstimator->getNumVisits(state,action);
  float mu = 0.5;
  float beta = nh / (n + nh + n * nh * b * b / (mu * (1.0 - mu)));
  float mainVal = mainValueEstimator->calcActionValue(state,action,false);
  float bound = mainValueEstimator->calcActionValue(state,action,useBounds) - mainVal;
  float val = (1 - beta) * mainVal + assistingValueEstimator->calcActionValue(state,action,false) + bound;
  return val;
}

template<class State, class Action>
Action DualUCTEstimator<State,Action>::selectAction(const State &state, bool useBounds) {
  std::vector<Action> maxActions;
  float maxVal = -BIGNUM;
  float val;

  for (Action a = (Action)0; a < numActions; a = Action(a+1)) {
    StateAction key(state,a);
    val = calcActionValue(state,a,useBounds);

    //std::cerr << val << " " << maxVal << std::endl;
    if (fabs(val - maxVal) < EPS)
      maxActions.push_back(a);
    else if (val > maxVal) {
      maxVal = val;
      maxActions.clear();
      maxActions.push_back(a);
    }
  }
  
  return maxActions[rng->randomInt(maxActions.size())];
}

#endif /* end of include guard: DUALUCTESTIMATOR_F3MKQ7CG */
