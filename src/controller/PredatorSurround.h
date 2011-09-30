#ifndef PREDATORSURROUND_YMRYX7FW
#define PREDATORSURROUND_YMRYX7FW

/*
File: PredatorSurround.h
Author: Samuel Barrett
Description: predator that surrounds the prey, not quite neighboring it, then tries to capture it suddenly
Created:  2011-09-29
Modified: 2011-09-29
*/

#include <queue>
#include "Agent.h"
#include "AStar.h"

class PredatorSurround: public Agent {
public:
  PredatorSurround(boost::shared_ptr<RNG> rng, const Point2D &dims);
  virtual ActionProbs step(const Observation &obs);
  virtual void restart(); // between episodes
  virtual std::string generateDescription();

protected:
  void setCaptureMode(const Observation &obs);
  Point2D getDesiredPosition(const Observation &obs);
  void assignDesiredDests(const Observation &obs);
  Point2D getMoveToPoint(const Point2D &start, const Point2D &end, bool &foundMove);

protected:
  static const int NUM_PREDATORS = 4;
  static const int NUM_DESTS = 4;
  
  AStar planner;
  bool captureMode;
  bool assignedDestsQ;
  Point2D destAssignments[NUM_PREDATORS];
  std::vector<Point2D> avoidLocations;
};

#endif /* end of include guard: PREDATORSURROUND_YMRYX7FW */
