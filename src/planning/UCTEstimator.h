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
#include <utility>
#include <cmath>
#include <boost/shared_ptr.hpp>

#include "ValueEstimator.h"
#include <common/RNG.h>
#include <common/DefaultMap.h>

#define BIGNUM 999999
#define EPS 1e-10

template<class State, class Action>
class UCTEstimator: public ValueEstimator<State,Action> {
public:
  typedef std::pair<State,Action> StateAction;

  UCTEstimator(boost::shared_ptr<RNG> rng, Action numActions, float lambda, float gamma, float rewardRangePerStep, float initialValue, float initialStateVisits, float initalStateActionVisits, float unseenValue);
  //UCTEstimator(boost::shared_ptr<RNG> rng, Action numActions, float lambda, float gamma, float rewardRangePerStep, float initialValue, float initialStateVisits, float initalStateActionVisits, float unseenValue);
  
  virtual Action selectWorldAction(const State &state);
  virtual Action selectPlanningAction(const State &state);
  virtual void startRollout(const State &state);
  virtual void finishRollout(bool terminal);
  virtual void visit(const Action &action, float reward, const State &state);
  virtual void restart();

protected:
  void checkInternals();
  float maxValueForState(const State &state);
  float updateStateAction(const StateAction &key, float newQ);

protected:
  boost::shared_ptr<RNG> rng;
  unsigned int numActions;
  float lambda;
  float gamma;
  float rewardBound;
  float unseenValue;

  bool valid;

  DefaultMap<StateAction,float> values;
  DefaultMap<State,unsigned int> stateVisits;
  DefaultMap<StateAction,unsigned int> stateActionVisits;
  
  DefaultMap<StateAction,unsigned int> rolloutVisitCounts;
  std::vector<State> historyStates;
  std::vector<Action> historyActions;
  std::vector<float> historyRewards;
};


////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
UCTEstimator<State,Action>::UCTEstimator(boost::shared_ptr<RNG> rng, Action numActions, float lambda, float gamma, float rewardRangePerStep, float initialValue, float initialStateVisits, float initialStateActionVisits, float unseenValue):
  rng(rng),
  numActions(numActions),
  lambda(lambda),
  gamma(gamma),
  unseenValue(unseenValue),
  valid(true),
  values(initialValue),
  stateVisits(initialStateVisits),
  stateActionVisits(initialStateActionVisits),
  rolloutVisitCounts(0)
{
  checkInternals();
  rewardBound = rewardRangePerStep / (1.0 - gamma);
}

template<class State, class Action>
void UCTEstimator<State,Action>::checkInternals() {
  if (numActions < 2) {
    std::cerr << "Invalid number of actions, must be at least 2" << std::endl;
    valid = false;
  }
  if ((lambda < 0) || (lambda > 1.0)) {
    std::cerr << "Invalid lambda: 0 <= lambda <= 1" << std::endl;
    valid = false;
  }
  if ((gamma < 0) || (gamma >= 1.0)) {
    std::cerr << "Invalid gamma: 0 <= gamma < 1" << std::endl;
    valid = false;
  }
}

template<class State, class Action>
void UCTEstimator<State,Action>::startRollout(const State &state) {
  historyStates.clear();
  historyActions.clear();
  historyRewards.clear();
  rolloutVisitCounts.clear();

  historyStates.push_back(state);
}

template<class State, class Action>
void UCTEstimator<State,Action>::visit(const Action &action, float reward, const State &state) {
  StateAction key(historyStates.back(),action);

  rolloutVisitCounts[key]++;
  historyActions.push_back(action);
  historyRewards.push_back(reward);
  historyStates.push_back(state);
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectWorldAction(const State &state) {
  std::vector<Action> maxActions;
  float maxVal = -BIGNUM;
  float val;
  unsigned int na;

  for (Action a = 0; a < numActions; a++) {
    StateAction key(state,a);
    na = stateActionVisits[key];

    if (na == 0)
      val = unseenValue;
    else
      val = values[key];

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
Action UCTEstimator<State,Action>::selectPlanningAction(const State &state) {
  std::vector<Action> maxActions;
  float maxVal = -BIGNUM;
  float val;
  unsigned int na;
  unsigned int n = stateVisits[state];
  //std::cout << "vals: ";

  for (Action a = 0; a < numActions; a++) {
    StateAction key(state,a);
    na = stateActionVisits[key];

    if (na == 0)
      val = unseenValue;
    else
      val = values[key] + rewardBound * sqrt(log(n) / na);
    //std::cout << val << "(" << values[key] << ") ";

    //std::cerr << val << " " << maxVal << std::endl;
    if (fabs(val - maxVal) < EPS)
      maxActions.push_back(a);
    else if (val > maxVal) {
      maxVal = val;
      maxActions.clear();
      maxActions.push_back(a);
    }
  }
  
  int ind = rng->randomInt(0,maxActions.size());
  //std::cout << "--> ";
  //for (int i = 0; i < maxActions.size(); i++)
    //std::cout << maxActions[i] << " ";
  //std::cout << " xxxx "<< ind << std::endl;
  //std::cerr << "ind = " << ind << " " << maxActions.size() << std::endl;
  return maxActions[ind];
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

  for (Action a = 0; a < numActions; a++) {
    StateAction key(state,a);
    val = values[key];
    if (val > maxVal)
      maxVal = val;
  }
  return maxVal;
}

template<class State, class Action>
float UCTEstimator<State,Action>::updateStateAction(const StateAction &key, float newQ){
  float learnRate = 1.0 / (1.0 + stateActionVisits[key]);
  //std::cout << "update(" << key.first <<"," << key.second << ") = " << values[key];
  stateVisits[key.first]++;
  stateActionVisits[key]++;
  values[key] += learnRate * (newQ - values[key]);
  //std::cout << " --> " << values[key] << std::endl;
  return lambda * newQ + (1.0 - lambda) * maxValueForState(key.first);
}

template<class State, class Action>
void UCTEstimator<State,Action>::finishRollout(bool terminal) {
  //std::cerr << "top finishRollout" << std::endl;
  float futureVal = 0;
  float newQ;

  //std::cerr << "original size: " << historyStates.size() << std::endl;
  if (!terminal)
    futureVal = maxValueForState(historyStates.back());
  historyStates.pop_back();

  for (int i = (int)historyStates.size() - 1; i >= 0; i--) {
    //std::cerr << "i = " << i << std::endl;
    StateAction key(historyStates[i],historyActions[i]);
    newQ = historyRewards[i] +  gamma * futureVal;
    if (rolloutVisitCounts[key] == 1)
      newQ = updateStateAction(key,newQ);
    rolloutVisitCounts[key]--;
    futureVal = newQ;
  }

  //std::cerr << "bottom finishRollout" << std::endl;
}

#endif /* end of include guard: UCTESTIMATOR_8N1RY426 */
