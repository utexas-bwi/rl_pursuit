#include "AgentFactory.h"

#include <cstdarg>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <controller/AgentRandom.h>
#include <controller/AgentDummy.h>
#include <controller/PredatorGreedy.h>
#include <controller/PredatorGreedyProbabilistic.h>
#include <controller/PredatorMCTS.h>
#include <controller/PredatorProbabilisticDestinations.h>
#include <controller/PredatorTeammateAware.h>
#include <controller/WorldMDP.h>
#include <planning/UCTEstimator.h>
#include <factory/PlanningFactory.h>

#define NAME_IN_SET(...) nameInSet(name,__VA_ARGS__,NULL)

bool nameInSet(const std::string &name, ...) {
  va_list vl;
  char *s;
  bool found = false;
  va_start(vl,name);
  while (true) {
    s = va_arg(vl,char *);
    if (s == NULL)
      break;
    else if (name == s) {
      found = true;
      break;
    }
  }
  va_end(vl);
  return found;
}

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, const Json::Value &, const Json::Value &rootOptions) {
  typedef boost::shared_ptr<Agent> ptr;
  
  boost::to_lower(name);
  if (NAME_IN_SET("prey","preyrandom","random"))
    return ptr(new AgentRandom(rng,dims));
  else if (NAME_IN_SET("greedy","gr"))
    return ptr(new PredatorGreedy(rng,dims));
  else if (NAME_IN_SET("greedyprobabilistic","greedyprob","gp"))
    return ptr(new PredatorGreedyProbabilistic(rng,dims));
  else if (NAME_IN_SET("probabilisticdestinations","probdests","pd"))
    return ptr(new PredatorProbabilisticDestinations(rng,dims));
  else if (NAME_IN_SET("teammate-aware","ta"))
    return ptr(new PredatorTeammateAware(rng,dims));
  else if (NAME_IN_SET("dummy"))
    return ptr(new AgentDummy(rng,dims));
  else if (NAME_IN_SET("mcts","uct")) {
    Json::Value plannerOptions = rootOptions["planner"];
    // process the depth if necessary
    unsigned int depth = plannerOptions.get("depth",0).asUInt();
    if (depth == 0) {
      unsigned int depthFactor = plannerOptions.get("depthFactor",0).asUInt();
      plannerOptions["depth"] = depthFactor * (dims.x + dims.y);
    }
    
    // create the mdp
    boost::shared_ptr<WorldMultiModelMDP> mdp = createWorldMultiModelMDP(rng,dims,plannerOptions);
    // create the value estimator
    boost::shared_ptr<UCTEstimator<State_t,Action::Type> > uct = createUCTEstimator(rng->randomUInt(),Action::NUM_ACTIONS,plannerOptions);
    // create the planner
    boost::shared_ptr<MCTS<State_t,Action::Type> > mcts = createMCTS(mdp,uct,plannerOptions);

    return ptr(new PredatorMCTS(rng,dims,mcts,mdp));
  } else {
    std::cerr << "createAgent: unknown agent name: " << name << std::endl;
    assert(false);
  }
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, const Json::Value &options, const Json::Value &rootOptions) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createAgent(rng,dims,name,options,rootOptions);
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, const Json::Value &options, const Json::Value &rootOptions) {
  std::string name = options.get("behavior","NONE").asString();
  if (name == "NONE") {
    std::cerr << "createAgent: WARNING: no agent type specified, using random" << std::endl;
    name = "random";
  }

  return createAgent(randomSeed,dims,name,options,rootOptions);
}
