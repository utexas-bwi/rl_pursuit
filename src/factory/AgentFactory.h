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
bool getStudentFromFile(const std::string &filename, std::string &student, unsigned int trialNum);
std::string getStudentForTrial(unsigned int trialNum, const Json::Value &options);

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions);
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions);
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions);

#endif /* end of include guard: AGENTFACTORY_31I9BDGK */
