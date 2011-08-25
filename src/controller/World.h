#ifndef WORLD_51O229LP
#define WORLD_51O229LP

#include <memory>
#include <vector>

/*
File: World.h
Author: Samuel Barrett
Description: the controller for the world
Created:  2011-08-22
Modified: 2011-08-22
*/

#include <boost/shared_ptr.hpp>
#include <common/Point2D.h>
#include <common/RNG.h>
#include <model/AgentModel.h>
#include <model/WorldModel.h>
#include "Agent.h"

class World {
public:
  World (boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world);
  
  void step();
  void generateObservation(Observation &obs);
  void handleCollisions(std::vector<Point2D> &requestedPositions);
  void randomizePositions();
  void restartAgents();
  bool addAgent(const AgentModel &agentModel, boost::shared_ptr<Agent> agent, bool ignorePosition=false);
  boost::shared_ptr<const WorldModel> getModel();

  std::string generateDescription(unsigned int indentation = 0);

protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> world;
  const Point2D dims;
  std::vector<boost::shared_ptr<Agent> > agents;
};

#endif /* end of include guard: WORLD_51O229LP */
