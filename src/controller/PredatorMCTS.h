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

class PredatorMCTS: public Agent {
public:
  PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<WorldMDP> model, boost::shared_ptr<ModelUpdater> modelUpdater);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  std::string generateLongDescription(unsigned int indentation = 0);

protected:
  boost::shared_ptr<MCTS<State_t,Action::Type> > planner;
  boost::shared_ptr<WorldMDP> model;
  boost::shared_ptr<ModelUpdater> modelUpdater;
  Observation prevObs;
  Action::Type prevAction;
};

#endif /* end of include guard: PREDATORMCTS_ERF6V5UK */
