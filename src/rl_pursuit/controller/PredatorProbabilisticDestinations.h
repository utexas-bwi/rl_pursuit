#ifndef PREDATORPROBABILISTICDESTINATIONS_NGJI2ZZP
#define PREDATORPROBABILISTICDESTINATIONS_NGJI2ZZP

/*
File: PredatorProbabilisticDestinations.h
Author: Samuel Barrett
Description: a predator that selects a random destination that's closer to the prey
Created:  2011-09-01
Modified: 2011-09-01
*/

#include "Agent.h"
#include "PredatorGreedy.h"

class PredatorProbabilisticDestinations: public Agent {
public:
  PredatorProbabilisticDestinations(boost::shared_ptr<RNG> rng, const Point2D &dims);
  ActionProbs step(const Observation &obs);

  void restart();
  std::string generateDescription();
  
  PredatorProbabilisticDestinations* clone() {
    return new PredatorProbabilisticDestinations(*this);
  }

private:
  void setDistanceProbs(unsigned distanceToPrey);
  void setDestinationsForDistance(const Observation &obs, int dist);
  void evaluateDestinations(const Observation &obs, float distanceProb);

private:
  std::vector<unsigned int> distances;
  std::vector<float> distanceProbs;
  std::vector<Point2D> destinations;
  ActionProbs actionProbs;

  static const float distanceFromPreyFactor;
  static const float distanceFromCurrentFactor;
};

#endif /* end of include guard: PREDATORPROBABILISTICDESTINATIONS_NGJI2ZZP */
