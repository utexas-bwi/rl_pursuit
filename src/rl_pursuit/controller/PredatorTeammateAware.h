#ifndef PREDATORTEAMMATEAWARE_78DZXW6S
#define PREDATORTEAMMATEAWARE_78DZXW6S

/*
File: PredatorTeammateAware.h
Author: Samuel Barrett
Description: a teammate aware predator - lets the farthest away predators select their destination first, then runs A* to reach the destination
Created:  2011-08-31
Modified: 2011-08-31
*/

#include "Agent.h"
#include "AStar.h"

const unsigned int NUM_PREDATORS = 4;
const unsigned int NUM_DESTS = Action::NUM_NEIGHBORS;

Point2D getTeammateAwareDesiredPosition(const Point2D &dims, const Observation &obs);
void assignTeammateAwareDesiredDests(const Point2D &dims, const Observation &obs, Point2D dests[NUM_PREDATORS], bool stopAfterAssigningCurrentPred, bool moveOntoPreyIfAtDest, int distFactor = 1);

class PredatorTeammateAware: public Agent {
public:
  PredatorTeammateAware(boost::shared_ptr<RNG> rng, const Point2D &dims);
  ActionProbs step(const Observation &obs);
  void restart(); // between episodes
  std::string generateDescription();
  
  PredatorTeammateAware* clone() {
    return new PredatorTeammateAware(*this);
  }

private:
  AStar planner;
};

#endif /* end of include guard: PREDATORTEAMMATEAWARE_78DZXW6S */
