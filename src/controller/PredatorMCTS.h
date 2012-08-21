#ifndef PREDATORMCTS_ERF6V5UK
#define PREDATORMCTS_ERF6V5UK

/*
File: PredatorMCTS.h
Author: Samuel Barrett
Description: a predator that uses MCTS to evaluate actions
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>

#include "Agent.h"
#include <planning/MCTS.h>
#include <controller/WorldMDP.h>
#include <controller/ModelUpdater.h>
#include <controller/QuandryDetector.h>
#include <controller/AStar.h>

#include <common/Params.h>

class PredatorMCTS: public Agent {
public:
#define PARAMS(_) \
  _(float,randomActionFrac,randomActionFrac,0.f)

  Params_STRUCT(PARAMS)
#undef PARAMS

public:
  PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<ModelUpdater> modelUpdater, boost::shared_ptr<QuandryDetector> quandryDetector, const Params &p);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  std::string generateLongDescription(unsigned int indentation = 0);

  PredatorMCTS* clone() {
    assert(false); // don't do this
    return new PredatorMCTS(*this);
  }

protected:
  boost::shared_ptr<MCTS<State_t,Action::Type> > planner;
  boost::shared_ptr<ModelUpdater> modelUpdater;
  boost::shared_ptr<QuandryDetector> quandryDetector;
  Observation prevObs;
  Action::Type prevAction;

  bool movingToTarget;
  Point2D target;
  AStar pathPlanner;

  Params p;
};

#endif /* end of include guard: PREDATORMCTS_ERF6V5UK */
