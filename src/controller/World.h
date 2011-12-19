#ifndef WORLD_51O229LP
#define WORLD_51O229LP

#include <memory>
#include <vector>

/*
File: World.h
Author: Samuel Barrett
Description: the controller for the world
Created:  2011-08-22
Modified: 2011-10-27
*/

#include <boost/shared_ptr.hpp>
#include <common/DefaultMap.h>
#include <common/Point2D.h>
#include <common/RNG.h>
#include <model/AgentModel.h>
#include <model/WorldModel.h>
#include "Agent.h"
#include "AgentDummy.h"

#include <gtest/gtest_prod.h>

struct WorldStepOutcome {
  Observation obs;
  double prob;
  Action::Type agentDummyAction;
};

class World {
public:
  World (boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world, double actionNoise, bool centerPrey);
  
  void generateObservation(Observation &obs);
  void step();
  void step(boost::shared_ptr<std::vector<Action::Type> > actions);
  void step(std::vector<boost::shared_ptr<Agent> > &agents);
  void step(boost::shared_ptr<std::vector<Action::Type> > actions, std::vector<boost::shared_ptr<Agent> > &agents);
  void randomizePositions();
  void restartAgents();
  bool addAgent(const AgentModel &agentModel, boost::shared_ptr<Agent> agent, bool ignorePosition=false);
  boost::shared_ptr<WorldModel> getModel();
  void setAgentControllers(const std::vector<boost::shared_ptr<Agent> > newAgents);

  std::string generateDescription(unsigned int indentation = 0);
  double getOutcomeProb(Observation prevObs,const Observation &currentObs);
  double getOutcomeProbApprox(Observation prevObs,const Observation &currentObs);//, std::vector<boost::shared_ptr<Agent> > &agents);
  void getPossibleOutcomesApprox(std::vector<AgentPtr> &agents, AgentPtr agentDummy, std::vector<std::vector<WorldStepOutcome> > &outcomesByAction);
  void printAgents();
  
  boost::shared_ptr<World> clone() const;
  virtual boost::shared_ptr<World> clone(const boost::shared_ptr<AgentDummy> &oldAdhocAgent, boost::shared_ptr<AgentDummy> &newAdhocAgent) const;

protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> world;
  const Point2D dims;
public:
  std::vector<boost::shared_ptr<Agent> > agents;
  double actionNoise;
  bool centerPrey;
protected:

protected:
  void handleCollisions(const std::vector<Point2D> &requestedPositions);
  void handleCollisionsOrdered(const std::vector<Point2D> &requestedPositions, const std::vector<unsigned int> &agentOrder);

  bool incrementActionIndices(std::vector<unsigned int> &actionInds);
  bool getRequestedPositionsForActionIndices(const std::vector<unsigned int> &actionInds, const std::vector<ActionProbs> &actionProbs, std::vector<Point2D> &requestedPositions);
  ActionProbs getAgentAction(unsigned int ind, boost::shared_ptr<Agent> agent, Observation &obs);
  double getProbOfNoCollisionApprox(const Observation &prevObs, const Observation &currentObs, const Point2D &requestedPosition, unsigned int agentInd);

  FRIEND_TEST(WorldTest,Collisions);
};

#endif /* end of include guard: WORLD_51O229LP */
