#include "AgentFactory.h"

#include <cstdarg>
#include <iostream>
#include <boost/algorithm/string.hpp>

#include <controller/Prey.h>
#include <controller/AgentDummy.h>
#include <controller/PredatorGreedy.h>
#include <controller/PredatorGreedyProbabilistic.h>
#include <controller/PredatorMCTS.h>
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

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, const Json::Value &options) {
  typedef boost::shared_ptr<Agent> ptr;
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  
  boost::to_lower(name);
  if (NAME_IN_SET("prey","preyrandom","random"))
    return ptr(new PreyRandom(rng,dims));
  else if (NAME_IN_SET("greedy","gr"))
    return ptr(new PredatorGreedy(rng,dims));
  else if (NAME_IN_SET("greedyprobabilistic","greedyprob","gp"))
    return ptr(new PredatorGreedyProbabilistic(rng,dims));
  else if (NAME_IN_SET("dummy"))
    return ptr(new AgentDummy(rng,dims));
  else if (NAME_IN_SET("mcts","uct")) {
    Json::Value plannerOptions = options["planner"];
    if (plannerOptions.isString()) {
      std::string filename = plannerOptions.asString();
      assert(readJson(filename,plannerOptions));
    }
    
    boost::shared_ptr<WorldMDP> mdp = createWorldMDP(rng,dims,plannerOptions);
    boost::shared_ptr<UCTEstimator<State_t,Action::Type> > uct = createUCTEstimator(rng->randomUInt(),Action::NUM_ACTIONS,plannerOptions);
    boost::shared_ptr<MCTS<State_t,Action::Type> > mcts = createMCTS(mdp,uct,plannerOptions);

    return ptr(new PredatorMCTS(rng,dims,mcts));
  } else {
    std::cerr << "createAgent: unknown agent name: " << name << std::endl;
    assert(false);
  }
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, const Json::Value &options) {
  std::string name = options.get("behavior","NONE").asString();
  if (name == "NONE") {
    std::cerr << "createAgent: WARNING: no agent type specified, using random" << std::endl;
    name = "random";
  }

  return createAgent(randomSeed,dims,name,options);
}
