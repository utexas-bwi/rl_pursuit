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
#include "ModelUpdater.h"
#include "StateMapping.h"
#include <rl_pursuit/common/Util.h>
#include <rl_pursuit/common/Params.h>

//#define MCTS_DEBUG
//#define MCTS_TIMINGS

#ifdef MCTS_DEBUG
#define MCTS_OUTPUT(x) std::cout << x << std::endl
#else
#define MCTS_OUTPUT(x) ((void) 0)
#endif

#ifdef MCTS_TIMINGS
#include <rl_pursuit/common/Enum.h>
#define MCTS_TIC(_) tic(MCTS_Timer::_)
#define MCTS_TOC(_) toc(MCTS_Timer::_)
#define MCTS_PRINT_TIMINGS() for(int i = 0; i < MCTS_Timer::NUM; i++) { \
  std::cout << getName((MCTS_Timer_t)i) << ": " << getTimer(i) << std::endl; \
}
#define MCTS_RESET_TIMINGS() for(int i = 0; i < MCTS_Timer::NUM; i++) { \
  resetTimer(i); \
}
ENUM(MCTS_Timer,
  TOTAL,
  SELECT_MODEL,
  SET_MODEL,
  START_ROLLOUT,
  SELECT_PLANNING_ACTION,
  TAKE_ACTION,
  VISIT,
  FINISH_ROLLOUT
)
#else
#define MCTS_TIC(_) (void)(0);
#define MCTS_TOC(_) (void)(0);
#define MCTS_PRINT_TIMINGS() (void)(0);
#define MCTS_RESET_TIMINGS() (void)(0);
#endif

template<class State, class Action>
class MCTS {
public:
  typedef boost::shared_ptr<MCTS<State,Action> > Ptr;
  typedef typename ValueEstimator<State,Action>::Ptr ValuePtr;
  typedef typename ModelUpdater<State,Action>::Ptr ModelUpdaterPtr;
  typedef typename Model<State,Action>::Ptr ModelPtr;
  typedef typename StateMapping<State>::Ptr StateMappingPtr;

#define PARAMS(_) \
  _(float,maxPlanningTime,maxPlanningTime,-1) \
  _(unsigned int,maxPlayouts,maxPlayouts,0) \
  _(unsigned int,maxDepth,maxDepth,0) \
  _(int,pruningMemorySize,pruningMemorySize,-1)

  Params_STRUCT(PARAMS)
#undef PARAMS

  MCTS (ValuePtr valueEstimator, ModelUpdaterPtr modelUpdater, StateMappingPtr stateMapping, const Params &p);
  virtual ~MCTS () {}

  unsigned int search(const State &startState);
  Action selectWorldAction(const State &state);
  void restart();
  std::string generateDescription(unsigned int indentation = 0);
  void pruneOldVisits() {
    valueEstimator->pruneOldVisits(p.pruningMemorySize);
  }

private:
  void checkInternals();
  void rollout(const State &startState);

private:
  ValuePtr valueEstimator;
  ModelUpdaterPtr modelUpdater;
  StateMappingPtr stateMapping;
  bool valid;
  double endPlanningTime;

  Params p;
};

////////////////////////////////////////////////////////////////////////////

template<class State, class Action>
MCTS<State,Action>::MCTS (ValuePtr valueEstimator, ModelUpdaterPtr modelUpdater, StateMappingPtr stateMapping, const Params &p):
  valueEstimator(valueEstimator),
  modelUpdater(modelUpdater),
  stateMapping(stateMapping),
  p(p)
{
  checkInternals();
}

template<class State, class Action>
unsigned int MCTS<State,Action>::search(const State &startState) {
  endPlanningTime = getTime() + p.maxPlanningTime;
  unsigned int playout;
  MCTS_RESET_TIMINGS();
  MCTS_TIC(TOTAL);
  for (playout = 0; (p.maxPlayouts == 0) || (playout < p.maxPlayouts); playout++) {
    MCTS_OUTPUT("-----------------------------------");
    MCTS_OUTPUT("ROLLOUT: " << playout);
    if ((p.maxPlanningTime > 0) && (getTime() > endPlanningTime))
      break;
    rollout(startState);
    MCTS_OUTPUT("-----------------------------------");
  }
  MCTS_TOC(TOTAL);
  MCTS_PRINT_TIMINGS();
  return playout;
}

template<class State, class Action>
Action MCTS<State,Action>::selectWorldAction(const State &state) {
  State mappedState(state);
  stateMapping->map(mappedState); // discretize state
  return valueEstimator->selectWorldAction(mappedState);
}

template<class State, class Action>
void MCTS<State,Action>::restart() {
  valueEstimator->restart();
}

template<class State, class Action>
std::string MCTS<State,Action>::generateDescription(unsigned int indentation) {
  std::stringstream ss;
  std::string prefix = indent(indentation);
  std::string prefix2 = indent(indentation + 1);
  ss << prefix  << "MCTS" << std::endl;
  ss << prefix2 << "num playouts: " << p.maxPlayouts << "\n";
  ss << prefix2 << "max planning time: " << p.maxPlanningTime << "\n";
  ss << prefix2 << "max depth: " << p.maxDepth << "\n";
  ss << prefix2 << "pruning memory size: " << p.pruningMemorySize << "\n";
  ss << prefix2 << "ValueEstimator:\n";
  ss << valueEstimator->generateDescription(indentation+2) << "\n";
  //ss << prefix << "Model:\n";
  //ss << model->generateDescription(indentation+1) << "\n"; // TODO removed during model change
  return ss.str();
}


template<class State, class Action>
void MCTS<State,Action>::checkInternals() {
  //if (p.maxPlanningTime < 0) {
    //std::cerr << "Invalid maxPlanningTime, must be >= 0" << std::endl;
    //valid = false;
  //}
  if ((p.maxPlayouts == 0) && (p.maxPlanningTime <= 0)) {
    std::cerr << "Must stop planning at some point, either specify maxPlayouts or maxPlanningTime" << std::endl;
    valid = false;
  }
}

template<class State, class Action>
void MCTS<State,Action>::rollout(const State &startState) {
  MCTS_OUTPUT("------------START ROLLOUT--------------");
  MCTS_TIC(SELECT_MODEL);
  ModelPtr model = modelUpdater->selectModel(startState);
  MCTS_TOC(SELECT_MODEL);
  State state(startState);
  State newState;
  Action action;
  float reward;
  bool terminal = false;
  MCTS_TIC(SET_MODEL);
  valueEstimator->setModel(model);
  MCTS_TOC(SET_MODEL);
  MCTS_TIC(START_ROLLOUT);
  valueEstimator->startRollout();
  MCTS_TOC(START_ROLLOUT);
  
  stateMapping->map(state); // discretize state

  for (unsigned int depth = 0; (depth < p.maxDepth) || (p.maxDepth == 0); depth++) {
    MCTS_OUTPUT("MCTS State: " << state << " " << "DEPTH: " << depth);
    if (terminal || ((p.maxPlanningTime > 0) && (getTime() > endPlanningTime)))
      break;
    MCTS_TIC(SELECT_PLANNING_ACTION);
    action = valueEstimator->selectPlanningAction(state);
    MCTS_OUTPUT("ACTION: " << action);
    MCTS_TOC(SELECT_PLANNING_ACTION);
    //std::cout << action << std::endl;
    MCTS_TIC(TAKE_ACTION);
    model->takeAction(action,reward,newState,terminal);
    MCTS_TOC(TAKE_ACTION);
    modelUpdater->updateSimulationAction(action,newState);
    MCTS_TIC(VISIT);
    valueEstimator->visit(state,action,reward);
    MCTS_TOC(VISIT);
    state = newState;
    stateMapping->map(state); // discretize state
  }

  MCTS_TIC(FINISH_ROLLOUT);
  valueEstimator->finishRollout(state,terminal);
  MCTS_TOC(FINISH_ROLLOUT);
  MCTS_OUTPUT("------------STOP  ROLLOUT--------------");
}

#endif /* end of include guard: MCTS_MJ647W13 */
