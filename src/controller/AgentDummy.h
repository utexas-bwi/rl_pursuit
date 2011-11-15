#ifndef AGENTDUMMY_G4R89AGK
#define AGENTDUMMY_G4R89AGK

/*
File: AgentDummy.h
Author: Samuel Barrett
Description: a dummy agent that return the last action that was set
Created:  2011-08-23
Modified: 2011-11-15
*/

#include "Agent.h"
#include <boost/lexical_cast.hpp>

class AgentDummy: public Agent {
public:
  AgentDummy(boost::shared_ptr<RNG> rng, const Point2D &dims, Action::Type action = Action::NOOP):
    Agent(rng,dims),
    action(action)
  {}

  ActionProbs step(const Observation &) { return ActionProbs(action); }
  void restart() {}
  std::string generateDescription() { return "AgentDummy: returns selected action, current max action: " + boost::lexical_cast<std::string>(action.maxAction()); }

  void setAction(Action::Type a) { action = ActionProbs(a); }
  void setAction(const ActionProbs &a) { action = a; }
  
  AgentDummy* clone() {
    return new AgentDummy(*this);
  }

private:
  ActionProbs action;
};

#endif /* end of include guard: AGENTDUMMY_G4R89AGK */
