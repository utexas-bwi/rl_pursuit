#ifndef AGENTRANDOM_2VL5554W
#define AGENTRANDOM_2VL5554W

/*
File: AgentRandom.h
Author: Samuel Barrett
Description: holds the prey behaviors
Created:  2011-08-22
Modified: 2011-08-30
*/

#include "Agent.h"
#include <iostream>

class AgentRandom: public Agent {
public:
  AgentRandom(boost::shared_ptr<RNG> rng, const Point2D &dims):
    Agent(rng,dims)
  {}

  ActionProbs step(const Observation &) {
    return ActionProbs(Action::RANDOM);
  }

  void restart() {}

  std::string generateDescription() { return "AgentRandom: a randomly moving agent"; }

  AgentRandom* clone() {
    return new AgentRandom(*this);
  }
};

#endif /* end of include guard: AGENTRANDOM_2VL5554W */
