#ifndef PREDATORGREEDYPROBABILISTIC_VHXUSHSP
#define PREDATORGREEDYPROBABILISTIC_VHXUSHSP

/*
File: PredatorGreedyProbabilistic.h
Author: Samuel Barrett
Description: a predator that's greedy with some randomness in its path planning
Created:  2011-08-30
Modified: 2011-08-30
*/

#include "Agent.h"
#include "PredatorGreedy.h"

class PredatorGreedyProbabilistic: public Agent {
public:
  PredatorGreedyProbabilistic(boost::shared_ptr<RNG> rng, const Point2D &dims);
  Action::Type step(const Observation &obs);

  void restart();
  std::string generateDescription();

private:
  static const unsigned int blockedPenalty;
};

#endif /* end of include guard: PREDATORGREEDYPROBABILISTIC_VHXUSHSP */
