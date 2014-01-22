#ifndef UCTESTIMATOR_8N1RY426
#define UCTESTIMATOR_8N1RY426

/*
File: UCTEstimator.h
Author: Samuel Barrett
Description: a value estimator based on UCT
Created:  2011-08-23
Modified: 2013-08-08
*/

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <utility>
#include <cmath>
#include <boost/foreach.hpp>
#include <boost/range/adaptor/map.hpp>
#include <boost/shared_ptr.hpp>

#include "ValueEstimator.h"
#include "Model.h"
#include <rl_pursuit/common/RNG.h>
#include <rl_pursuit/common/DefaultMap.h>
#include <rl_pursuit/common/Util.h>
#include <rl_pursuit/common/Params.h>

#ifdef UCT_DEBUG
#define UCT_OUTPUT(x) std::cout << x << std::endl
#else
#define UCT_OUTPUT(x) ((void) 0)
#endif

template<class State, class Action>
class UCTEstimator: public ValueEstimator<State, Action> {
public:
  typedef boost::shared_ptr<UCTEstimator<State,Action> > Ptr;
  typedef std::pair<State,Action> StateAction;
  
  struct StateActionInfo {
    StateActionInfo(unsigned int visits, float val):
      visits(visits),
      val(val),
      rolloutVisits(0)
    {
    }
    unsigned int visits;
    float val;
    unsigned int rolloutVisits;
    std::map<State,unsigned int> next_state_visits;
    std::map<State,float> next_state_val;
  };

  struct StateInfo {
    StateInfo(unsigned int stateVisits):
      stateVisits(stateVisits),
      lastVisit(-1)
    {
    }
    std::map<Action,StateActionInfo> actionInfos;
    unsigned int stateVisits;
    int lastVisit;
  };
  
  typedef typename std::map<State,StateInfo>::iterator StateIter;
  typedef typename std::map<Action,StateActionInfo>::iterator StateActionIter;

  struct HistoryStep {
    HistoryStep(const State &state, const Action &action, float reward, StateActionInfo *stateActionInfo, StateInfo *stateInfo):
      state(state),
      action(action),
      reward(reward),
      stateActionInfo(stateActionInfo),
      stateInfo(stateInfo)
    {}

    State state;
    Action action;
    float reward;
    StateActionInfo *stateActionInfo;
    StateInfo *stateInfo;
  };

#define PARAMS(_) \
  _(float,lambda,lambda,0.9) \
  _(float,gamma,gamma,0.9) \
  _(float,rewardBound,rewardBound,-1) \
  _(float,rewardRangePerStep,rewardRangePerStep,-1) \
  _(float,initialValue,initialValue,0) \
  _(unsigned int,initialStateVisits,initialStateVisits,0) \
  _(unsigned int,initialStateActionVisits,initialStateActionVisits,0) \
  _(float,unseenValue,unseenValue,999999) \
  _(bool,theoreticallyCorrectLambda,theoreticallyCorrectLambda,false) \
  _(bool,useImportanceSampling,useImportanceSampling,false)

  Params_STRUCT(PARAMS);
#undef PARAMS

  UCTEstimator(boost::shared_ptr<RNG> rng, const Params &p);
  
  virtual Action selectWorldAction(const State &state);
  virtual Action selectPlanningAction(const State &state);
  virtual void startRollout();
  virtual void finishRollout(const State &state,bool terminal);
  virtual void visit(const State &state, const Action &action, float reward);
  virtual void restart();
  virtual std::string generateDescription(unsigned int indentation = 0);
  void pruneOldVisits(int memorySize); // 0 keeps none, -1 prunes nothing

protected:
  virtual float maxValueForState(const State &state, StateInfo *stateInfo);
  virtual float calcActionValue(StateActionInfo *stateActionInfo, StateInfo *stateInfo, bool useBounds);
  void checkInternals();
  virtual Action selectAction(const State &state, bool useBounds);
  float updateStateAction(const State &state, const Action &action, const State &next_state, StateActionInfo *stateActionInfo, StateInfo *stateInfo, float newQ);
  void printValues(const State &state);

protected:
  boost::shared_ptr<RNG> rng;
  Params p;
  bool valid;

  std::map<State,StateInfo> stateInfos;

  //DefaultMap<StateAction,float> values;
  //DefaultMap<State,unsigned int> stateVisits;
  //DefaultMap<StateAction,unsigned int> stateActionVisits;
  //DefaultMap<State,int> lastStateVisit;
  int numPruneCalls;
  
  //DefaultMap<StateAction,unsigned int> rolloutVisitCounts;
  std::vector<HistoryStep> history;
public:
  static const float EPS;
  static const float BIGNUM;
};

template<class State, class Action>
const float UCTEstimator<State,Action>::EPS = 1e-10;
template<class State, class Action>
const float UCTEstimator<State,Action>::BIGNUM = 999999;

template<class State, class Action>
std::ostream& operator<<(std::ostream &out, const std::pair<State,Action> &sa) {
  out << "<SA " << sa.first << " " << sa.second << ">";
  return out;
}

////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
UCTEstimator<State,Action>::UCTEstimator(boost::shared_ptr<RNG> rng, const Params &np):
  rng(rng),
  p(np),
  valid(true),
  //values(p.initialValue),
  //stateVisits(p.initialStateVisits),
  //stateActionVisits(p.initialStateActionVisits),
  ////stateActions(std::make_pair(initialValue,initialStateActionVisits)),
  //lastStateVisit(-1),
  numPruneCalls(0)
  //rolloutVisitCounts(0)
{
  if (p.rewardBound > 0) {
    if (p.rewardRangePerStep > 0) {
      std::cerr << "UCTEstimator: ERROR, both rewardBound and rewardRangePerStep > 0, which one do you want?" << std::endl;
      valid = false;
    }
  } else {
    if (p.rewardRangePerStep <= 0) {
      std::cerr << "UCTEstimator: ERROR, both rewardBound and rewardRangePerStep <= 0, must specify at least one" << std::endl;
      valid = false;
    }
    p.rewardBound = p.rewardRangePerStep / (1.0 - p.gamma);
  }

  checkInternals();
}
  

template<class State, class Action>
void UCTEstimator<State,Action>::checkInternals() {
  if ((p.lambda < 0) || (p.lambda > 1.0)) {
    std::cerr << "UCTEstimator: Invalid lambda: 0 <= lambda <= 1" << std::endl;
    valid = false;
  }
  if ((p.gamma < 0) || (p.gamma > 1.0)) {
    std::cerr << "UCTEstimator: Invalid gamma: 0 <= gamma <= 1" << std::endl;
    valid = false;
  }
  assert(valid);
}

template<class State, class Action>
void UCTEstimator<State,Action>::startRollout() {
  history.clear();
  //rolloutVisitCounts.clear();
}

template<class State, class Action>
void UCTEstimator<State,Action>::visit(const State &state, const Action &action, float reward) {
  StateIter it = stateInfos.find(state);
  if (it == stateInfos.end()) {
    std::pair<StateIter,bool> res;
    res = stateInfos.insert(std::pair<State,StateInfo>(state,StateInfo(p.initialStateVisits)));
    it = res.first;
    //std::cout << "INSERTED STATE" << std::endl;
    //std::cout << "  VIS: " << stateInfos.find(state)->second.stateVisits << std::endl;
  }
  StateInfo *stateInfo = &(it->second);

  StateActionIter ita = stateInfo->actionInfos.find(action);
  if (ita == stateInfo->actionInfos.end()) {
    std::pair<StateActionIter,bool> res;
    res = stateInfo->actionInfos.insert(std::pair<Action,StateActionInfo>(action,StateActionInfo(p.initialStateActionVisits,p.initialValue)));
    ita = res.first;
    //std::cout << "INSERTED ACTION" << std::endl;
    //std::cout << "  VIS: " << stateInfo->actionInfos.find(action)->second.visits << std::endl;
  }
  StateActionInfo *stateActionInfo = &(ita->second);
  stateActionInfo->rolloutVisits++;
  history.push_back(HistoryStep(state,action,reward,stateActionInfo,stateInfo));
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectAction(const State &state, bool useBounds) {
  StateIter it = stateInfos.find(state);
  if (it == stateInfos.end()) {
    //std::cout << "selectAction: NULL" << std::endl;
    // select randomly
    std::vector<Action> maxActions;
    Action a;
    bool actionValid = true;
    for (this->model->getFirstAction(state,a); actionValid; actionValid = this->model->getNextAction(state,a)) {
      maxActions.push_back(a);
    }
    return maxActions[rng->randomInt(maxActions.size())];
  }

  StateInfo *stateInfo = &(it->second);
  std::vector<Action> maxActions;
  float maxVal = -BIGNUM;
  float val;

  Action a;
  bool actionValid = true;
  //std::cout << "selectAction: ";
  for (this->model->getFirstAction(state,a); actionValid; actionValid = this->model->getNextAction(state,a)) {
    StateActionIter ita = stateInfo->actionInfos.find(a);
    StateActionInfo *stateActionInfo = &(ita->second);
    if (ita == stateInfo->actionInfos.end()) {
      // unseen state action
      stateActionInfo = NULL;
      //std::cout << "UNKNOWN ACTION" << std::endl;
    }
    //std::cout << "VISI: " << ita->second.visits << std::endl;
    val = calcActionValue(stateActionInfo,stateInfo,useBounds);
    //std::cout << a << "(" << val << ") ";
    //if (!useBounds) {
      //std::cout << state << " " << a << " " << val << std::endl;
    //}

    //std::cerr << val << " " << maxVal << std::endl;
    if (fabs(val - maxVal) < EPS)
      maxActions.push_back(a);
    else if (val > maxVal) {
      maxVal = val;
      maxActions.clear();
      maxActions.push_back(a);
    }
  }
  //std::cout << std::endl;

  //if (!useBounds) {
    //std::cout << "MAX ACTIONS(" << " " << maxActions.size() << "): " << std::endl;
    //for (unsigned int i= 0; i < maxActions.size(); i++)
      //std::cout << maxActions[i] << " ";
    //std::cout << std::endl;
  //}
  
  return maxActions[rng->randomInt(maxActions.size())];
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectWorldAction(const State &state) {
#ifdef UCT_DEBUG
  printValues(state);
#endif
  Action action = selectAction(state,false);
  return action;
}

template<class State, class Action>
Action UCTEstimator<State,Action>::selectPlanningAction(const State &state) {
  //if (history.size() == 0)
    //printValues(state);
  Action action = selectAction(state,true);
  //std::cout << state << " " << action << std::endl;
  return action;
}

template<class State, class Action>
void UCTEstimator<State,Action>::restart() {
  stateInfos.clear();
}

template<class State, class Action>
float UCTEstimator<State,Action>::maxValueForState(const State &state, StateInfo *stateInfo) {
  if (stateInfo == NULL)
    return p.initialValue;

  float maxVal = -BIGNUM;

  Action a;
  bool actionValid = true;
  for (this->model->getFirstAction(state,a); actionValid; actionValid = this->model->getNextAction(state,a)) {
    StateActionIter ita = stateInfo->actionInfos.find(a);
    StateActionInfo *stateActionInfo = &(ita->second);
    if (ita == stateInfo->actionInfos.end()) {
      // unseen state action
      stateActionInfo = NULL;
    }
    float val = calcActionValue(stateActionInfo,stateInfo,false);
    if (val > maxVal)
      maxVal = val;
  }
  return maxVal;
}

template<class State, class Action>
float UCTEstimator<State,Action>::updateStateAction(const State &state, const Action& action, const State &next_state, StateActionInfo *stateActionInfo, StateInfo *stateInfo, float newQ){
  //std::cout << "update(" << key.first <<"," << key.second << ") = " << values[key];
  stateInfo->stateVisits++;
  stateActionInfo->visits++;
  stateInfo->lastVisit = numPruneCalls;
  float retVal = 0;
  if (!p.useImportanceSampling) {
    float learnRate = 1.0 / (stateActionInfo->visits);
    if (p.theoreticallyCorrectLambda)
      retVal = p.lambda * newQ + (1.0 - p.lambda) * maxValueForState(state,stateInfo);

    stateActionInfo->val += learnRate * (newQ - stateActionInfo->val);
    
    if (!p.theoreticallyCorrectLambda)
      retVal = p.lambda * newQ + (1.0 - p.lambda) * maxValueForState(state,stateInfo);
    //std::cout << " --> " << values[key] << std::endl;
  } else {
    if (stateActionInfo->next_state_visits.find(next_state) ==
        stateActionInfo->next_state_visits.end()) {
      stateActionInfo->next_state_visits[next_state] = 0;
      stateActionInfo->next_state_val[next_state] = 0.0f;
    }
    stateActionInfo->next_state_visits[next_state]++;
    // float learnRate = 1.0 / (stateActionInfo->next_state_visits[next_state]);
    // stateActionInfo->next_state_val[next_state] += 
    //   learnRate * (newQ - stateActionInfo->next_state_val[next_state]);
    stateActionInfo->next_state_val[next_state] = newQ;

    float probability_sum = 0;
    float value_sum = 0;
    BOOST_FOREACH(const State& next_state, 
        stateActionInfo->next_state_val | boost::adaptors::map_keys) {
      //float probability = this->model->getTransitionProbability(state, action, next_state);
      float probability = 
        ((float) stateActionInfo->next_state_visits[next_state]) /
        ((float) stateActionInfo->visits);
      probability_sum += probability;
      value_sum += probability * stateActionInfo->next_state_val[next_state];
    }

    float value = value_sum / probability_sum;
    if (p.theoreticallyCorrectLambda)
      retVal = p.lambda * value + (1.0 - p.lambda) * maxValueForState(state,stateInfo);

    stateActionInfo->val = value;
    
    if (!p.theoreticallyCorrectLambda)
      retVal = p.lambda * value + (1.0 - p.lambda) * maxValueForState(state,stateInfo);
  }
  return retVal;
}

template<class State, class Action>
void UCTEstimator<State,Action>::finishRollout(const State &state, bool terminal) {
  float futureVal;
  float newQ;

  StateIter it = stateInfos.find(state);
  StateInfo *stateInfo = &(it->second);
  if (it == stateInfos.end())
    stateInfo = NULL;

  if (terminal)
    futureVal = 0;
  else
    futureVal = maxValueForState(state,stateInfo);

  State next_state = state;
  for (int i = (int)history.size() - 1; i >= 0; i--) {
    StateAction key(history[i].state,history[i].action);
    newQ = history[i].reward +  p.gamma * futureVal;
    if ((history[i].stateActionInfo->rolloutVisits) == 1) {
      newQ = updateStateAction(history[i].state,history[i].action,next_state,history[i].stateActionInfo,history[i].stateInfo,newQ);
    }
    history[i].stateActionInfo->rolloutVisits--;
    futureVal = newQ;
    next_state = history[i].state;
  }
}

template<class State, class Action>
std::string UCTEstimator<State,Action>::generateDescription(unsigned int indentation) {
  std::stringstream ss;
  ss << indent(indentation) + "UCTEstimator:\n";
  std::string prefix = indent(indentation+1);
  ss << prefix << "lambda: " << p.lambda << "\n";
  ss << prefix << "gamma: " << p.gamma << "\n";
  ss << prefix << "rewardBound: " << p.rewardBound << "\n";
  if (p.theoreticallyCorrectLambda)
    ss << prefix << "theoreticallyCorrectLambda" << "\n";
  else
    ss << prefix << "empiricalLambda" << "\n";
  ss << prefix << "unseenValue: " << p.unseenValue;// << "\n";
  return ss.str();
}
  
template<class State, class Action>
float UCTEstimator<State,Action>::calcActionValue(StateActionInfo *stateActionInfo, StateInfo *stateInfo, bool useBounds) {
  //std::cout << "calcActionValue for " << state << " " << action << " " << useBounds << std::endl;
  if (stateActionInfo == NULL) {
    if (useBounds)
      return p.unseenValue;
    else
      return -BIGNUM;//p.initialValue;
  }
  //std::cout << "calcActionValue: " << stateInfo->stateVisits << " " << stateActionInfo->visits << " " << stateActionInfo->val << std::endl;
  if (useBounds) {
    unsigned int &na = stateActionInfo->visits;
    unsigned int &n = stateInfo->stateVisits;

    if (na == 0)
      return p.unseenValue;
    else {
      return stateActionInfo->val + p.rewardBound * sqrt(log(n) / na);
    }
  } else
    return stateActionInfo->val;
}

template<class State, class Action>
void UCTEstimator<State,Action>::pruneOldVisits(int memorySize) {
  // 1 keeps the most recent, 0 keeps none, <0 means no pruning
  numPruneCalls++;
  if (memorySize < 0) {
    return;
  }

  typename std::map<State,StateInfo>::iterator it = stateInfos.begin();
  while (it != stateInfos.end()) {
    if (it->second.lastVisit < numPruneCalls - memorySize) { //always true if memorySize is 0 because numPruneCalls is incremented above
      stateInfos.erase(it++); // NOTE: the ++ must be like this because the we erase the copy, after incrementing the original
    } else {
      ++it;
    }
  }
}
  
template<class State, class Action>
void UCTEstimator<State,Action>::printValues(const State &state) {
  StateIter it = stateInfos.find(state);
  if (it == stateInfos.end()) {
    UCT_OUTPUT("No values to print");
    return;
  }
  StateInfo *stateInfo = &(it->second);
  Action a;
  bool actionValid = true;
  std::stringstream ss;
  ss << "Vals(" << stateInfo->stateVisits << "): ";
  for (this->model->getFirstAction(state,a); actionValid; actionValid = this->model->getNextAction(state,a)) {
    unsigned int numVisits = 0;
    StateActionInfo *stateActionInfo = NULL;
    StateActionIter ita = stateInfo->actionInfos.find(a);
    if (ita != stateInfo->actionInfos.end())
      stateActionInfo = &(ita->second);
    if (stateActionInfo != NULL)
      numVisits = stateActionInfo->visits;
    ss << calcActionValue(stateActionInfo,stateInfo,false) << "(" << numVisits << ")";
    typedef std::pair<State, unsigned int> StateUIPair;
    if (p.useImportanceSampling) {
      ss << "-";
      std::stringstream ss2;
      unsigned int total_visits = 0;
      float experential_value = 0.0f;
      BOOST_FOREACH(const StateUIPair &v, stateActionInfo->next_state_visits) {
        ss2 << stateActionInfo->next_state_val[v.first] << "/" << v.second << ",";
        total_visits += v.second;
        experential_value += stateActionInfo->next_state_val[v.first] * v.second;
      }
      experential_value /= total_visits;
      ss << "(" << experential_value << "/" << total_visits << ")-(" <<
        ss2.str() << ")";
    }
  }
  UCT_OUTPUT(ss.str());
}

#endif /* end of include guard: UCTESTIMATOR_8N1RY426 */
