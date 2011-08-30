#ifndef MCTS_MJ647W13
#define MCTS_MJ647W13

/*
File: MCTS.h
Author: Samuel Barrett
Description: a monte-carlo tree search
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include "Model.h"
#include "ValueEstimator.h"
#include <common/Util.h>

template<class State, class Action>
class MCTS {
public:
  MCTS (boost::shared_ptr<Model<State,Action> > model, boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth);
  virtual ~MCTS () {}

  void search(const State &startState);
  Action selectWorldAction(const State &state);
  void restart();
  std::string generateDescription(unsigned int indentation = 0);

private:
  void checkInternals();
  void rollout(const State &startState);

private:
  boost::shared_ptr<Model<State,Action> > model;
  boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator;
  unsigned int numPlayouts;
  double maxPlanningTime;
  unsigned int maxDepth;
  bool valid;
  double endPlanningTime;
};

////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
MCTS<State,Action>::MCTS(boost::shared_ptr<Model<State,Action> > model, boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth):
  model(model),
  valueEstimator(valueEstimator),
  numPlayouts(numPlayouts),
  maxPlanningTime(maxPlanningTime),
  maxDepth(maxDepth)
{
  checkInternals();
}

template<class State, class Action>
void MCTS<State,Action>::search(const State &startState) {
  endPlanningTime = getTime() + maxPlanningTime;
  for (unsigned int i = 0; (numPlayouts == 0) || (i < numPlayouts); i++) {
    //std::cout << "-----------------------------------" << std::endl;
    //std::cout << "ROLLOUT: " << i << std::endl;
    if ((maxPlanningTime > 0) && (getTime() > endPlanningTime))
      break;
    rollout(startState);
    //std::cout << "-----------------------------------" << std::endl;
  }
}

template<class State, class Action>
Action MCTS<State,Action>::selectWorldAction(const State &state) {
  return valueEstimator->selectWorldAction(state);
}

template<class State, class Action>
void MCTS<State,Action>::restart() {
  valueEstimator->restart();
}

template<class State, class Action>
std::string MCTS<State,Action>::generateDescription(unsigned int indentation) {
  std::stringstream ss;
  std::string prefix = indent(indentation);
  ss << prefix << "num playouts: " << numPlayouts << "\n";
  ss << prefix << "max planning time: " << maxPlanningTime << "\n";
  ss << prefix << "max depth: " << maxDepth << "\n";
  ss << prefix << "ValueEstimator:\n";
  ss << valueEstimator->generateDescription(indentation+1) << "\n";
  ss << prefix << "Model:\n";
  ss << model->generateDescription(indentation+1) << "\n";
  return ss.str();
}


template<class State, class Action>
void MCTS<State,Action>::checkInternals() {
  if (maxPlanningTime < 0) {
    std::cerr << "Invalid maxPlanningTime, must be >= 0" << std::endl;
    valid = false;
  }
  if ((numPlayouts == 0) && (maxPlanningTime <= 0)) {
    std::cerr << "Must stop planning at some point, either specify numPlayouts or maxPlanningTime" << std::endl;
    valid = false;
  }
}

template<class State, class Action>
void MCTS<State,Action>::rollout(const State &startState) {
  std::cout << "--------------------------" << std::endl;
  State state(startState);
  Action action;
  float reward;
  bool terminal = false;
  model->setState(startState);
  valueEstimator->startRollout(startState);

  for (unsigned int depth = 0; (depth < maxDepth) || (maxDepth == 0); depth++) {
    std::cout << state << std::endl;
    //std::cout << "DEPTH: " << depth << std::endl;
    if (terminal || ((maxPlanningTime > 0) && (getTime() > endPlanningTime)))
      break;
    action = valueEstimator->selectPlanningAction(state);
    model->takeAction(action,reward,state,terminal);
    valueEstimator->visit(action,reward,state);
  }

  valueEstimator->finishRollout(terminal);
}

#endif /* end of include guard: MCTS_MJ647W13 */
