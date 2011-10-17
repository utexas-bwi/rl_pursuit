#ifndef PREDATORGREEDY_BSWV5ETY
#define PREDATORGREEDY_BSWV5ETY

/*
File: PredatorGreedy.h
Author: Samuel Barrett
Description: a greedy predator
Created:  2011-08-22
Modified: 2011-08-22
*/

#include "Agent.h"
#include <model/Common.h>

Point2D getGreedyDesiredPosition(const Point2D &dims, const Observation &obs);
Action::Type greedyObstacleAvoid(const Point2D &dims, const Observation &obs, const Point2D &dest);

class PredatorGreedy: public Agent {
public:
  PredatorGreedy(boost::shared_ptr<RNG> rng, const Point2D &dims):
    Agent(rng,dims)
  { }

  ActionProbs step(const Observation &obs);

  void restart() {};
  std::string generateDescription() { return "PredatorGreedy: a predator that greedily pursues the prey";}
  
  PredatorGreedy* clone() {
    return new PredatorGreedy(*this);
  }
};

#endif /* end of include guard: PREDATORGREEDY_BSWV5ETY */
