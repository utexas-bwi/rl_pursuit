#ifndef PREDATORSURROUNDWITHPENALTIES_KDKLSIF4
#define PREDATORSURROUNDWITHPENALTIES_KDKLSIF4

/*
File: PredatorSurroundWithPenalties.h
Author: Samuel Barrett
Description: predator that surrounds the prey, not quite neighboring it, then tries to capture it suddenly, with penalties when teammates break this behavior
Created:  2011-09-29
Modified: 2011-09-29
*/

#include "PredatorSurround.h"
#include <deque>

class PredatorSurroundWithPenalties: public PredatorSurround {
public:
  PredatorSurroundWithPenalties(boost::shared_ptr<RNG> rng, const Point2D &dims);
  ActionProbs step(const Observation &obs);
  void restart(); // between episodes
  std::string generateDescription();
  
  PredatorSurroundWithPenalties* clone() {
    return new PredatorSurroundWithPenalties(*this);
  }

private:
  void setPenaltyMode(const Observation &obs);
  void setExpectedMoves(const Observation &obs);

private:
  static const float penaltyAmount;
  static const unsigned int violationHistorySize;
  
  bool penaltyOn;
  bool usePrevObs;
  std::deque<int> violationHistory;
  Action::Type expectedMoves[NUM_PREDATORS];
  Observation prevObs;
};

#endif /* end of include guard: PREDATORSURROUNDWITHPENALTIES_KDKLSIF4 */
