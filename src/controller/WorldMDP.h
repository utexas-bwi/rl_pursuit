#ifndef WORLDMDP_CNHINAVX
#define WORLDMDP_CNHINAVX

/*
File: WorldMDP.h
Author: Samuel Barrett
Description: an mdp wrapper of a world
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>

#include <planning/Model.h>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>
#include <controller/AgentDummy.h>

class WorldMDP: public Model<Observation,Action::Type> {
public:
  WorldMDP(boost::shared_ptr<RNG> rng,const Point2D &dims,const Json::Value &options);

  void setState(const Observation &state);
  void takeAction(const Action::Type &action, float &reward, Observation &state, bool &terminal);

private:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> worldModel;
  boost::shared_ptr<World> worldController;
  boost::shared_ptr<AgentDummy> adhocAgent;
};

#endif /* end of include guard: WORLDMDP_CNHINAVX */
