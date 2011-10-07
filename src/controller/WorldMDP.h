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
#include <ostream>

#include <planning/Model.h>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>
#include <controller/AgentDummy.h>
#include <controller/State.h>

class ModelUpdater;

class WorldMDP: public Model<State_t,Action::Type> {
public:
  WorldMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent);

  virtual void setState(const State_t &state);
  virtual void setState(const Observation &obs);
  virtual State_t getState(const Observation &obs);

  virtual void takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal);
  virtual float getRewardRangePerStep();
  virtual std::string generateDescription(unsigned int indentation = 0);
  void setAgents(const std::vector<boost::shared_ptr<Agent> > &agents);
  double getOutcomeProb(const Observation &prevObs, Action::Type adhocAction, const Observation &currentObs);
  boost::shared_ptr<AgentDummy> getAdhocAgent();
  virtual void addAgent(AgentType agentType, boost::shared_ptr<Agent> agent);
  Point2D getDims() {
    return model->getDims();
  }
  virtual void setBeliefs(boost::shared_ptr<ModelUpdater> ) {
    // do nothing :)
  }


protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> model;
  boost::shared_ptr<World> controller;
  boost::shared_ptr<AgentDummy> adhocAgent;
};

#endif /* end of include guard: WORLDMDP_CNHINAVX */
