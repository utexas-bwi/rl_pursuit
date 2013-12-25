#ifndef AGENTFACTORY_31I9BDGK
#define AGENTFACTORY_31I9BDGK

/*
File: AgentFactory.h
Author: Samuel Barrett
Description: generates agent controllers
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <string>
#include <vector>
#include <set>
#include <boost/shared_ptr.hpp>
#include <rl_pursuit/json/json.h>
#include <rl_pursuit/controller/Agent.h>
#include <rl_pursuit/controller/AgentPerturbation.h>
#include <rl_pursuit/controller/QuandryDetector.h>
#include <rl_pursuit/common/RNG.h>

bool nameInSet(const std::string &name, ...);
bool getStudentFromFile(const std::string &filename, std::string &student, unsigned int trialNum);
std::string getStudentForTrial(unsigned int trialNum, const Json::Value &options);
void getAvailableStudents(const std::string &filename, std::set<std::string> &students);
AgentPerturbation::Perturbation getPerturbation(unsigned int trialNum, int predatorInd, const Json::Value &options);
void getPerturbationFromFile(unsigned int trialNum, int predatorInd, const std::string &filename, float &amount, std::string &typeStr);

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent = boost::shared_ptr<Agent>());
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent = boost::shared_ptr<Agent>());
boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent = boost::shared_ptr<Agent>());

boost::shared_ptr<QuandryDetector> createQuandryDetector(const Point2D &dims, const Json::Value &options);

#endif /* end of include guard: AGENTFACTORY_31I9BDGK */
