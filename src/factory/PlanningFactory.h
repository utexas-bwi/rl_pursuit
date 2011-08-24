#ifndef PLANNINGFACTORY_4TYHDV2K
#define PLANNINGFACTORY_4TYHDV2K

/*
File: PlanningFactory.h
Author: Samuel Barrett
Description: generates objects for planning
Created:  2011-08-24
Modified: 2011-08-24
*/

#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <controller/WorldMDP.h>
#include <planning/UCTEstimator.h>
#include <planning/UCTEstimator.h>
#include <planning/MCTS.h>

// WORLD MDPs
boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, const Json::Value &options);

// UCT ESTIMATORS

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, float lambda, float gamma, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initalStateActionVisits, float unseenValue);

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, float rewardRangePerStep, const Json::Value &options);

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(unsigned int randomSeed, Action::Type numActions, float rewardRangePerStep, const Json::Value &options);

// MCTS
boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth);

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,const Json::Value &options);

#endif /* end of include guard: PLANNINGFACTORY_4TYHDV2K */
