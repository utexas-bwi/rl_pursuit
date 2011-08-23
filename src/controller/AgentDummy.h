#ifndef AGENTDUMMY_G4R89AGK
#define AGENTDUMMY_G4R89AGK

/*
File: AgentDummy.h
Author: Samuel Barrett
Description: a dummy agent that return the last action that was set
Created:  2011-08-23
Modified: 2011-08-23
*/

#include "Agent.h"

class AgentDummy: public Agent {
public:
  AgentDummy(boost::shared_ptr<RNG> rng, const Point2D &dims):
    Agent(rng,dims),
    action(Action::NOOP)
  {}

  Action::Type step(const Observation &) { return action; }
  void restart() {}
  std::string generateDescription() { return "AgentDummy: returns the last action that was set"; }

  void setAction(Action::Type a) { action = a; }

private:
  Action::Type action;
};

#endif /* end of include guard: AGENTDUMMY_G4R89AGK */
