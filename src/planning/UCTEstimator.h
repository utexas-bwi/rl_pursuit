#ifndef UCTESTIMATOR_8N1RY426
#define UCTESTIMATOR_8N1RY426

/*
File: UCTEstimator.h
Author: Samuel Barrett
Description: a value estimator based on UCT
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <cmath>
#include <boost/shared_ptr.hpp>

#include "ValueEstimator.h"
#include <common/RNG.h>
#include <common/DefaultMap.h>
#include <common/Util.h>

#define BIGNUM 999999
#define EPS 1e-10

template<class State, class Action>
class UCTEstimator: public ValueEstimator<State, Action> {
public:
  typedef std::pair<State,Action> StateAction;
  struct HistoryStep {
    HistoryStep(const State &state, const Action &action, float reward):
      state(state),
      action(action),
      reward(reward)
    {}

    State state;
    Action action;
    float reward;
  };

  UCTEstimator(boost::shared_ptr<RNG> rng, Action numActions, float lambda, float gamma, float rewardBound, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initalStateActionVisits, float unseenValue);
  
  virtual Action selectWorldAction(const State &state);
  virtual Action selectPlanningAction(const State &state);
  virtual void startRollout();
  virtual void finishRollout(const State &state,bool terminal);
  virtual void visit(const State &state, const Action &action, float reward);
  virtual void restart();
  virtual std::string generateDescription(unsigned int indentation = 0);
  float maxValueForState(const State &state);
  float calcActionValue(const State &state, const Action &action, bool useBounds);

protected:
  void checkInternals();
  virtual Action selectAction(const State &state, bool useBounds);
  float updateStateAction(const StateAction &key, float newQ);

protected:
  boost::shared_ptr<RNG> rng;
  Action numActions;
  float lambda;
  float gamma;
  float rewardBound;
  float unseenValue;

  bool valid;

  DefaultMap<StateAction,float> values;
  DefaultMap<State,unsigned int> stateVisits;
  DefaultMap<StateAction,unsigned int> stateActionVisits;
  //DefaultMap<StateAction,std::pair<float,unsigned int> > stateActions;
  
  DefaultMap<StateAction,unsigned int> rolloutVisitCounts;
  std::vector<HistoryStep> history;
};

template<class State, class Action>
std::ostream& operator<<(std::ostream &out, const std::pair<State,Action> &sa) {
  out << "<SA " << sa.first << " " << sa.second << ">";
  return out;
}

////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
UCTEstimator<State,Action>::UCTEstimator(boost::shared_ptr<RNG> rng, Action numActions, float lambda, float gamma, float nrewardBound, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initialStateActionVisits, float unseenValue):
  rng(rng),
  numActions(numActions),
  lambda(lambda),
  gamma(gamma),
  unseenValue(unseenValue),
  valid(true),
  values(initialValue),
  stateVisits(initialStateVisits),
  stateActionVisits(initialStateActionVisits),
  //stateActions(std::make_pair(initialValue,initialStateActionVisits)),
  rolloutVisitCounts(0)
{
  if (nrewardBound > 0) {
    if (rewardRangePerStep > 0) {
      std::cerr << "UCTEstimator: ERROR, both rewardBound and rewardRangePerStep > 0, which one do you want?" << std::endl;
      valid = false;
    }
    rewardBound = nrewardBound;
  } else {
    if (rewardRangePerStep <= 0) {
      std::cerr << "UCTEstimator: ERROR, both rewardBound and rewardRangePerStep <= 0, must specify at least one" << std::endl;
      valid = false;
    }
    rewardBound = rewardRangePerStep / (1.0 - gamma);
  }

  checkInternals();
}
  

template<class State, class Action>
void UCTEstimator<State,Action>::checkInternals() {
  if (numActions < 2) {
    std::cerr << "UCTEstimator: Invalid number of actions, must be at least 2" << std::endl;
    valid = false;
  }
  if ((lambda < 0) || (lambda > 1.0)) {
    std::cerr << "UCTEstimator: Invalid lambda: 0 <= lambda <= 1" << std::endl;
    valid = false;
  }
  if ((gamma < 0) || (gamma > 1.0)) {
    std::cerr << "UCTEstimator: Invalid gamma: 0 <= gamma <= 1" << std::endl;
    valid = false;
  }
  assert(valid);
}

template<class State, class Action>
void UCTEstimator<State,Action>::startRollout() {
  //static int i = 0;
  //i++;
  //i %= 1000;
  //if (i == 0)
    //std::cout << stateVisits.size() << " " << stateActions.size() [><< " " << values.size()<] << std::endl;
  history.clear();
  rolloutVisitCounts.clear();

  //historyStates.push_back(state);
}

template<class State, class Action>
void UCTEstimator<State,Action>::visit(const State &state, const Action &action, float reward) {
  rolloutVisitCounts[StateAction(state,action)]++;
  history.push_back(HistoryStep(state,action,reward));
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectAction(const State &state, bool useBounds) {
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
  
  return maxActions[rng->randomInt(0,maxActions.size())];
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectWorldAction(const State &state) {
  //std::cout << state << ": ";
  //for (int i = 0; i < 5; i++) {
    //std::cout << " " << getStateActionValue(state,(Action)i);
  //}
  Action action = selectAction(state,false);
  //std::cout << " : " << action;
  //std::cout << std::endl;
  return action;
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectPlanningAction(const State &state) {
  return selectAction(state,true);
}

template<class State, class Action>
void UCTEstimator<State,Action>::restart() {
  values.clear();
  stateVisits.clear();
  stateActionVisits.clear();
}

template<class State, class Action>
float UCTEstimator<State,Action>::maxValueForState(const State &state) {
  float maxVal = -BIGNUM;
  float val;

  for (Action a = (Action)0; a < numActions; a = Action(a+1)) {
    StateAction key(state,a);
    val = values.get(key);
    if (val > maxVal)
      maxVal = val;
  }
  return maxVal;
}

template<class State, class Action>
float UCTEstimator<State,Action>::updateStateAction(const StateAction &key, float newQ){
  float learnRate = 1.0 / (1.0 + stateActionVisits.get(key));
  //std::cout << "update(" << key.first <<"," << key.second << ") = " << values[key];
  stateVisits[key.first]++;
  stateActionVisits[key]++;
  float retVal = lambda * newQ + (1.0 - lambda) * maxValueForState(key.first);
  values[key] += learnRate * (newQ - values[key]); // TODO previous version had these 2 lines swapped, but I think this is correct
  //std::cout << " --> " << values[key] << std::endl;
  return retVal;
}

template<class State, class Action>
void UCTEstimator<State,Action>::finishRollout(const State &state, bool terminal) {
  //std::cerr << "top finishRollout" << std::endl;
  float futureVal;
  float newQ;

  //std::cerr << "original size: " << historyStates.size() << std::endl;
  if (terminal)
    futureVal = 0;
  else
    futureVal = maxValueForState(state);

  for (int i = (int)history.size() - 1; i >= 0; i--) {
    //std::cerr << "i = " << i << std::endl;
    //std::cout << "FUTURE VAL: " << futureVal << std::endl;
    StateAction key(history[i].state,history[i].action);
    newQ = history[i].reward +  gamma * futureVal;
    if (rolloutVisitCounts.get(key) == 1)
      newQ = updateStateAction(key,newQ);
    rolloutVisitCounts[key]--;
    futureVal = newQ;
  }

  //std::cerr << "bottom finishRollout" << std::endl;
}

template<class State, class Action>
std::string UCTEstimator<State,Action>::generateDescription(unsigned int indentation) {
  std::stringstream ss;
  ss << indent(indentation) + "UCTEstimator:\n";
  std::string prefix = indent(indentation+1);
  ss << prefix << "lambda: " << lambda << "\n";
  ss << prefix << "gamma: " << gamma << "\n";
  ss << prefix << "rewardBound: " << rewardBound << "\n";
  ss << prefix << "unseenValue: " << unseenValue;// << "\n";
  return ss.str();
}
  
template<class State, class Action>
float UCTEstimator<State,Action>::calcActionValue(const State &state, const Action &action, bool useBounds) {
  unsigned int na;
  StateAction key = StateAction(state,action);
  if (useBounds) {
    na = stateActionVisits.get(key);

    if (na == 0)
      return unseenValue;
    else {
      unsigned int n = stateVisits.get(state);
      return values.get(key) + rewardBound * sqrt(log(n) / na);
    }
  } else
    return values.get(key);
}

#endif /* end of include guard: UCTESTIMATOR_8N1RY426 */
