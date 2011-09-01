/*
File: PredatorProbabilisticDestinations.cpp
Author: Samuel Barrett
Description: a predator that selects a random destination that's closer to the prey
Created:  2011-09-01
Modified: 2011-09-01
*/
#include "PredatorProbabilisticDestinations.h"

const float PredatorProbabilisticDestinations::distanceFromPreyFactor = -1; // -1 -> prefer smaller distance, closer to the prey
const float PredatorProbabilisticDestinations::distanceFromCurrentFactor = -1; // -1 -> prefer smaller distance, closer my current position

PredatorProbabilisticDestinations::PredatorProbabilisticDestinations(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims)
{}

ActionProbs PredatorProbabilisticDestinations::step(const Observation &obs) {
  actionProbs.reset();

  unsigned int distanceToPrey = getDistanceToPoint(dims,obs.myPos(),obs.preyPos());

  if (distanceToPrey == 1) {
    // if right next to the prey, move onto it
    Action::Type action = getAction(getDifferenceToPoint(dims,obs.myPos(),obs.preyPos()));
    return ActionProbs(action);
  }

  setDistanceProbs(distanceToPrey);
  for (unsigned int i = 0; i < distances.size(); i++) {
    setDestinationsForDistance(obs,distances[i]);
    evaluateDestinations(obs,distanceProbs[i]);
  }
  
  return actionProbs;
}

void PredatorProbabilisticDestinations::restart() {
}

std::string PredatorProbabilisticDestinations::generateDescription() {
  return "PredatorProbabilisticDestinations: a predator that selects a random destination that's closer to the prey";
}

void PredatorProbabilisticDestinations::setDistanceProbs(unsigned distanceToPrey) {
  distances.clear();
  unsigned int maxDist = min(dims.x,dims.y) / 2; // don't go bigger than half the world
  maxDist = min(maxDist,distanceToPrey - 1); // make the destination closer to the prey than we are
  for (unsigned int i = 1; i <= maxDist; i++)
    distances.push_back(i);
  softmax(distances,distanceFromPreyFactor,distanceProbs);
}

void PredatorProbabilisticDestinations::setDestinationsForDistance(const Observation &obs, int dist) {
  destinations.clear();
  Point2D diff(dist,0);
  Point2D change(-1,1); // keeps us on the diamond
  while (diff.x > -dist) {
    destinations.push_back(movePosition(dims,obs.preyPos(),diff));
    if (diff.y == dist)
      change.y *= -1;
    diff += change;
  }
  change.x *= -1;
  while (diff.x < dist) {
    destinations.push_back(movePosition(dims,obs.preyPos(),diff));
    if (diff.y == -dist)
      change.y *= -1;
    diff += change;
  }
}
  
void PredatorProbabilisticDestinations::evaluateDestinations(const Observation &obs, float distanceProb) {
  Point2D diff;
  Point2D chosenMove;
  Point2D nextPos;
  std::vector<Action::Type> moves;
  std::vector<unsigned int> destDists;
  std::vector<float> destProbs;

  for (unsigned int i = 0; i < destinations.size(); i++) {
    // if the destination is occupied, don't choose it
    if (obs.getCollision(destinations[i]) >= 0)
      continue;
    diff = getDifferenceToPoint(dims,obs.myPos(),destinations[i]);
    if (abs(diff.x) > abs(diff.y)) {
      chosenMove.x = sgn(diff.x);
      chosenMove.y = 0;
    } else {
      chosenMove.x = 0;
      chosenMove.y = sgn(diff.y);
    }
    // check if the next position is occupied, if it is, don't choose it
    nextPos = movePosition(dims,obs.myPos(),chosenMove);
    assert(nextPos != obs.myPos()); // really shouldn't happen
    if (obs.getCollision(nextPos) >= 0)
      continue;
    moves.push_back(getAction(chosenMove));
    destDists.push_back(diff.manhattanDist());
  } // end for

  if (destDists.size() == 0) {
    // no valid destinations, just move randomly
    for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
      actionProbs[(Action::Type)i] += distanceProb / Action::NUM_ACTIONS;
  } else {
    softmax(destDists,distanceFromCurrentFactor,destProbs);
    for (unsigned int i = 0; i < destProbs.size(); i++)
      actionProbs[moves[i]] += distanceProb * destProbs[i];
  }
}
