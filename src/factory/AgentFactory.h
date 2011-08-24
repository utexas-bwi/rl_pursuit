#ifndef AGENTFACTORY_31I9BDGK
#define AGENTFACTORY_31I9BDGK

/*
File: AgentFactory.h
Author: Samuel Barrett
Description: generates agent controllers
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include <controller/Agent.h>
#include <common/RNG.h>

bool nameInSet(const std::string &name, ...);

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, const Json::Value &options = Json::Value());
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, const Json::Value &options = Json::Value());
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, const Json::Value &options);

#endif /* end of include guard: AGENTFACTORY_31I9BDGK */
