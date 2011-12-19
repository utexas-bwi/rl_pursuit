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
  WorldMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, bool usePreySymmetry);
  
  virtual void setPreyPos(const Point2D &preyPos);
  virtual void setState(const State_t &state);
  virtual void setState(const Observation &obs);
  virtual State_t getState(const Observation &obs);

  virtual void takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal);
  virtual void step(Action::Type adhocAction); //, std::vector<boost::shared_ptr<Agent> > &agents);
  virtual void learnControllers(const Observation &prevObs, const Observation &currentObs);
  virtual float getRewardRangePerStep();
  virtual std::string generateDescription(unsigned int indentation = 0);
  void setAgents(const std::vector<boost::shared_ptr<Agent> > &agents);
  //void saveAgents();
  //void loadAgents();
  double getOutcomeProb(const Observation &prevObs, Action::Type adhocAction, const Observation &currentObs);//, std::vector<boost::shared_ptr<Agent> > &agents);
  boost::shared_ptr<AgentDummy> getAdhocAgent();
  virtual void addAgent(const AgentModel &agentModel, boost::shared_ptr<Agent> agent);
  virtual void addAgents(const std::vector<AgentModel> &agentModels, const std::vector<boost::shared_ptr<Agent> > agents);
  Point2D getDims() const {
    return model->getDims();
  }
  virtual void setBeliefs(boost::shared_ptr<ModelUpdater> ) {
    // do nothing :)
  }

  virtual boost::shared_ptr<WorldMDP> clone() const;
  void setAdhocAgent(boost::shared_ptr<AgentDummy> adhocAgent);


protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> model;
  boost::shared_ptr<World> controller;
  boost::shared_ptr<AgentDummy> adhocAgent;

  Point2D preyPos;
  bool usePreySymmetry;
  static const bool useCaching;

  friend class WorldMDPTest;
  friend class ModelUpdaterBayesTest;
};

#endif /* end of include guard: WORLDMDP_CNHINAVX */
