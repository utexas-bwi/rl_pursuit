#ifndef MCTS_MJ647W13
#define MCTS_MJ647W13

/*
File: MCTS.h
Author: Samuel Barrett
Description: a monte-carlo tree search
Created:  2011-08-23
Modified: 2011-12-13
*/

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <sstream>
#include "Model.h"
#include "ValueEstimator.h"
#include <common/Util.h>
#include <controller/ModelUpdater.h>

template<class State, class Action>
class MCTS {
public:
  MCTS (boost::shared_ptr<Model<State,Action> > model, boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator, boost::shared_ptr<ModelUpdater> modelUpdater, unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth, int pruningMemorySize);
  virtual ~MCTS () {}

  void search(const State &startState);
  Action selectWorldAction(const State &state);
  void restart();
  std::string generateDescription(unsigned int indentation = 0);
  void pruneOldVisits() {
    valueEstimator->pruneOldVisits(pruningMemorySize);
  }

private:
  void checkInternals();
  void rollout(const State &startState);

private:
  boost::shared_ptr<Model<State,Action> > model;
  boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator;
  boost::shared_ptr<ModelUpdater> modelUpdater;
  unsigned int numPlayouts;
  double maxPlanningTime;
  unsigned int maxDepth;
  bool valid;
  double endPlanningTime;
  int pruningMemorySize;
};

////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
MCTS<State,Action>::MCTS(boost::shared_ptr<Model<State,Action> > model, boost::shared_ptr<ValueEstimator<State,Action> > valueEstimator, boost::shared_ptr<ModelUpdater> modelUpdater, unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth, int pruningMemorySize):
  model(model),
  valueEstimator(valueEstimator),
  modelUpdater(modelUpdater),
  numPlayouts(numPlayouts),
  maxPlanningTime(maxPlanningTime),
  maxDepth(maxDepth),
  pruningMemorySize(pruningMemorySize)
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
  ss << prefix << "pruning memory size: " << pruningMemorySize << "\n";
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
  //std::cout << "------------START ROLLOUT--------------" << std::endl;
  modelUpdater->selectModel(startState);
  State state(startState);
  State newState;
  Action action;
  float reward;
  bool terminal = false;
  //std::cout << "start mcts set state" << std::endl;
  model->setState(startState);
  //std::cout << "stop  mcts set state" << std::endl;
  valueEstimator->startRollout();

  for (unsigned int depth = 0; (depth < maxDepth) || (maxDepth == 0); depth++) {
    //std::cout << "MCTS State: " << state << " ";// << std::endl;
    //std::cout << "DEPTH: " << depth << std::endl;
    if (terminal || ((maxPlanningTime > 0) && (getTime() > endPlanningTime)))
      break;
    action = valueEstimator->selectPlanningAction(state);
    //std::cout << action << std::endl;
    model->takeAction(action,reward,newState,terminal);
    modelUpdater->updateSimulationAction(action,newState);
    valueEstimator->visit(state,action,reward);
    state = newState;
  }

  valueEstimator->finishRollout(state,terminal);
  //std::cout << "------------STOP  ROLLOUT--------------" << std::endl;
}

#endif /* end of include guard: MCTS_MJ647W13 */
