#ifndef PREY_2VL5554W
#define PREY_2VL5554W

/*
File: Prey.h
Author: Samuel Barrett
Description: holds the prey behaviors
Created:  2011-08-22
Modified: 2011-08-22
*/

#include "Agent.h"
#include <iostream>

class PreyRandom: public Agent {
public:
  PreyRandom(boost::shared_ptr<RNG> rng, const Point2D &dims):
    Agent(rng,dims)
  {}

  Action::Type step(const Observation &) {
    return (Action::Type)(rng->randomInt(Action::NUM_ACTIONS));
  }

  void restart() {}

  std::string generateDescription() { return "PreyRandom: a randomly moving prey agent"; }
};

#endif /* end of include guard: PREY_2VL5554W */
