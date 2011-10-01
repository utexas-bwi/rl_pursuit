/*
File: PredatorSurroundWithPenalties.cpp
Author: Samuel Barrett
Description: predator that surrounds the prey, not quite neighboring it, then tries to capture it suddenly, with penalties when teammates break this behavior
Created:  2011-09-29
Modified: 2011-09-29
*/

#include "PredatorSurroundWithPenalties.h"

#include <model/Common.h>
#include "PredatorGreedy.h"

const float PredatorSurroundWithPenalties::penaltyAmount = 0.7;
const unsigned int PredatorSurroundWithPenalties::violationHistorySize = 5;

PredatorSurroundWithPenalties::PredatorSurroundWithPenalties(boost::shared_ptr<RNG> rng, const Point2D &dims):
  PredatorSurround(rng,dims),
  penaltyOn(false),
  usePrevObs(false)
{
}

ActionProbs PredatorSurroundWithPenalties::step(const Observation &obs) {
  // get the action without penalties
  ActionProbs action = PredatorSurround::step(obs);
  // check whether we should penalize
  setPenaltyMode(obs);
  // set the expected moves for this step
  setExpectedMoves(obs);

  // optionally apply penalty
  if ((penaltyOn) && (!captureMode)){
    for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
      action[(Action::Type)i] *= (1.0 - penaltyAmount);
    action[Action::NOOP] += penaltyAmount;
  }

  usePrevObs = (!captureMode && !isStuck);
  prevObs = obs;

  return action;
}


void PredatorSurroundWithPenalties::restart() {
  PredatorSurround::restart();
  penaltyOn = false;
  usePrevObs = false;
  violationHistory.clear();
}

std::string PredatorSurroundWithPenalties::generateDescription() {
  return "PredatorSurroundWithPenalties: surrounds the prey at a distance before trying to capture it and punished predators that break this behavior";
}
  
void PredatorSurroundWithPenalties::setPenaltyMode(const Observation &obs) {
  int stepViolations = 0;
  if (!captureMode && usePrevObs) {
    for (int i = 0; i < NUM_PREDATORS; i++) {
      if (i+1 == (int)obs.myInd) // +1 because prey is 0
        continue;
      if (expectedMoves[i] == Action::NUM_ACTIONS)
        continue; // wasn't sure what that guy was going to do
      Point2D move = getDifferenceToPoint(dims,prevObs.positions[i+1],obs.positions[i+1]);
      Point2D desiredPosition = movePosition(dims,prevObs.positions[i+1],expectedMoves[i]);
      bool desiredPositionOccupied = false;
      for (unsigned int j = 0; j < obs.positions.size(); j++)
        if (desiredPosition == obs.positions[j])
          desiredPositionOccupied = true;
      if (desiredPositionOccupied)
        continue;
      if (move != Action::MOVES[expectedMoves[i]]) {
        stepViolations++;
        break;
      }
    }
  }

  // add the violation count to the history
  while (violationHistory.size() >= violationHistorySize)
    violationHistory.pop_front();
  violationHistory.push_back(stepViolations);
  
  // check the number of violations in history
  int numViolations = 0;
  for (unsigned int i = 0; i < violationHistory.size(); i++)
    numViolations += violationHistory[i];
  if (numViolations >= 2)
    penaltyOn = true;
  else
    penaltyOn = false;
}

void PredatorSurroundWithPenalties::setExpectedMoves(const Observation &obs) {
  // get the desired destination
  if (!assignedDestsQ)
    assignDesiredDests(obs);

  // avoidLocations is already set correctly
  for (int i = 0; i < NUM_PREDATORS; i++) {
    if (destAssignments[i] == obs.positions[i+1]) {
      expectedMoves[i] = Action::NOOP;
      continue;
    }
    bool foundMove;
    Point2D move = getMoveToPoint(obs.positions[i+1],destAssignments[i],foundMove);

    if (foundMove)
      expectedMoves[i] = getAction(move);
    else
      expectedMoves[i] = Action::NUM_ACTIONS;
  }
}
