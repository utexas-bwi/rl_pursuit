#include "AgentFactory.h"

#include <cstdarg>
#include <boost/algorithm/string.hpp>

#include <controller/Prey.h>
#include <controller/PredatorGreedy.h>
#include <controller/PredatorMCTS.h>
#include <planning/UCTEstimator.h>

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
  if (nameInSet(name,"prey","preyrandom","random",NULL))
    return ptr(new PreyRandom(rng,dims));
  else if (nameInSet(name,"greedy","gr",NULL))
    return ptr(new PredatorGreedy(rng,dims));
  //else if (nameInSet(name,"mcts","uct",NULL)) {
    //boost::shared_ptr<UCTEstimator<Observation,Action::Type> > valueEstimator = new UCTEstimator<Observation,Action::Type>(rng,Action::NUM_ACTIONS,options);
    //boost::shared_ptr<MCTS<Observation,Action::Type> > planner = new MCTS<Observation,Action::Type>(model,valueEstimator,options);
    //return ptr(new PredatorMCTS(rng,dims,planner));
  //}
  else {
    std::cerr << "createAgent: unknown agent name: " << name << std::endl;
    assert(false);
  }
}
