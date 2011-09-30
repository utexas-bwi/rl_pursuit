#ifndef PREYAVOIDNEIGHBOR_45JS10ZE
#define PREYAVOIDNEIGHBOR_45JS10ZE

/*
File: PreyAvoidNeighbor.h
Author: Samuel Barrett
Description: a prey that avoids neighboring predators and noops or moves randomly otherwise
Created:  2011-09-30
Modified: 2011-09-30
*/

#include "Agent.h"

class PreyAvoidNeighbor: public Agent {
public:
  PreyAvoidNeighbor(boost::shared_ptr<RNG> rng, const Point2D &dims);
  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();

private:
  void getNeighborMoves(const Observation &obs, std::vector<Point2D> &neighborMoves);
  ActionProbs moveWithNoNeighbors();

private:
  static const float noopWeight;
};

#endif /* end of include guard: PREYAVOIDNEIGHBOR_45JS10ZE */
