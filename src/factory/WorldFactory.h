#ifndef WORLDFACTORY_NNNSUN1M
#define WORLDFACTORY_NNNSUN1M

/*
File: WorldFactory.h
Author: Samuel Barrett
Description: generates a world model controller
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>
#include "AgentFactory.h"

int getReplacementInd(unsigned int trialNum);

// world models
boost::shared_ptr<WorldModel> createWorldModel(const Point2D &dims);

// world model + controller
boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model);
boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, const Point2D &dims);
boost::shared_ptr<World> createWorld(unsigned int randomSeed, boost::shared_ptr<WorldModel> model);

// just agents
void createAgentControllersAndModels(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options, std::vector<boost::shared_ptr<Agent> > &agentControllers, std::vector<AgentModel> &agentModels);
void createAgentControllersAndModels(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options, boost::shared_ptr<Agent> adhocAgent, std::vector<boost::shared_ptr<Agent> > &agentControllers, std::vector<AgentModel> &agentModels);

// world model + controller + agents
boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<World> world, unsigned int trialNum, const Json::Value &options);
boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, unsigned int trialNum, const Json::Value &options);
boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, const Json::Value &options);
boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, const Json::Value &options);
boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, unsigned int trialNum, const Json::Value &options);
boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, boost::shared_ptr<WorldModel> model, unsigned int trialNum, const Json::Value &options);

// helpers
Point2D getDims(const Json::Value &options);

#endif /* end of include guard: WORLDFACTORY_NNNSUN1M */
