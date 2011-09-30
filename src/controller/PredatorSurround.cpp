/*
File: PredatorSurround.cpp
Author: Samuel Barrett
Description: predator that surrounds the prey, not quite neighboring it, then tries to capture it suddenly
Created:  2011-09-29
Modified: 2011-09-29
*/

#include "PredatorSurround.h"

#include <model/Common.h>
#include "PredatorGreedy.h"

PredatorSurround::PredatorSurround(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims),
  planner(dims),
  captureMode(false)
{
}

ActionProbs PredatorSurround::step(const Observation &obs) {
  //std::cout << obs << std::endl;
  assignedDestsQ = false;
  avoidLocations = obs.positions; // reset the avoid locations to the current positions of agents
  // don't get too close to the prey
  for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) 
    avoidLocations.push_back(movePosition(dims,obs.preyPos(),(Action::Type)i));
  // get the capture mode
  setCaptureMode(obs);
  //if (captureMode)
    //std::cout << "CAPTURE MODE ENGAGED" << std::endl;
  //else
    //std::cout << "DO NOT CAPTURE" << std::endl;

  Point2D desiredPosition = getDesiredPosition(obs);
  //std::cout << "avoidLocations: ";
  //for (unsigned int i = 0; i < avoidLocations.size(); i++)
    //std::cout << avoidLocations[i] << " ";
  //std::cout << std::endl;
  //for (unsigned int i = 1; i < avoidLocations.size(); i++) { // 1 because the prey is 0
    //if (desiredPosition == avoidLocations[i])
      //destinationBlocked = true;
  //}
  
  bool foundMove;
  Point2D diff = getMoveToPoint(obs.myPos(),desiredPosition,foundMove);
  if (foundMove) {
    return ActionProbs(getAction(diff));
  } else {
    return ActionProbs(Action::RANDOM);
  }
}

Point2D PredatorSurround::getMoveToPoint(const Point2D &start, const Point2D &end, bool &foundMove) {
  bool destinationBlocked = false;
  Point2D diff;
  if (start == end) {
    //diff = Point2D(0,0);
    foundMove = false;
    return Point2D(0,0);
  } else {
    if (!destinationBlocked) {
      planner.plan(start,end,avoidLocations);
      destinationBlocked = !planner.foundPath();
    }
    if (destinationBlocked) {
      //std::cout << "NO PATH FOUND, moving randomly: " << obs << " " << dest << std::endl;
      foundMove = false;
      return Point2D(0,0);
    }
    diff = getDifferenceToPoint(dims,start,planner.getFirstStep());
  }
  foundMove = true;
  return diff;
}

Point2D PredatorSurround::getDesiredPosition(const Observation &obs) {
  assert(obs.preyInd == 0);

  if (captureMode) {
    return getGreedyDesiredPosition(dims,obs);
  } else {
    // get the desired destination
    assignDesiredDests(obs);
    //std::cout << "destAssignments: ";
    //for (unsigned int i = 0; i < NUM_DESTS; i++)
      //std::cout << destAssignments[i] << " ";
    //std::cout << std::endl;
    return destAssignments[obs.myInd-1]; // -1 because prey is 1
  }  
}

void PredatorSurround::restart() {
  captureMode = false;
}

std::string PredatorSurround::generateDescription() {
  return "PredatorSurround: surrounds the prey at a distance before trying to capture it";
}

void PredatorSurround::setCaptureMode(const Observation &obs) {
  unsigned int dist;
  for (unsigned int i = 0; i < obs.positions.size(); i++) {
    if ((int)i == obs.preyInd)
      continue;
    dist = getDistanceToPoint(dims,obs.positions[i],obs.preyPos());
    if (dist > 2) {
      captureMode = false;
      return;
    }
  }
  captureMode = true;
}

void PredatorSurround::assignDesiredDests(const Observation &obs) {
  assignedDestsQ = true;
  // FIXME assuming prey is in position 0
  assert(obs.preyInd == 0);
  // FIXME assuming 4 predators and 1 prey
  assert(obs.positions.size() == NUM_PREDATORS + 1);

  int distances[NUM_PREDATORS][NUM_DESTS];
  Point2D possibleDests[NUM_DESTS];
  
  // get the possible destinations and the dists to the predators
  for (int destInd = 0; destInd < NUM_DESTS; destInd++) {
    possibleDests[destInd] = movePosition(dims,obs.preyPos(),2 * Action::MOVES[destInd]);
    for (int pred = 0; pred < NUM_PREDATORS; pred++) {
      distances[pred][destInd] = getDistanceToPoint(dims,obs.positions[pred + 1],possibleDests[destInd]); // +1 because prey is in position 0
    }
  }

  for (int numUnassignedPredators = NUM_PREDATORS; numUnassignedPredators > 0; numUnassignedPredators--) {
    // calculate the distance from each dest to the nearest predator
    int minDists[NUM_DESTS];
    int minInds[NUM_DESTS];
    for (int destInd = 0; destInd < NUM_DESTS; destInd++) {
      minDists[destInd] = 999999;
      for (int predInd = 0; predInd < NUM_PREDATORS; predInd++) {
        if (distances[predInd][destInd] < minDists[destInd]) {
          minDists[destInd] = distances[predInd][destInd];
          minInds[destInd] = predInd;
        }
      }
    }

    // select the dest with the largest dist to a predator
    int maxDist = -1;
    int chosenDestInd = -1;
    for (int destInd = 0; destInd < NUM_DESTS; destInd++) {
      if (minDists[destInd] > maxDist) {
        maxDist = minDists[destInd];
        chosenDestInd = destInd;
      }
    }
    int chosenPredInd = minInds[chosenDestInd];
    destAssignments[chosenPredInd] = possibleDests[chosenDestInd];
    // remove this predator and dest from consideration
    for (int destInd = 0; destInd < NUM_DESTS; destInd++)
      distances[chosenPredInd][destInd] = 999999;
    for (int predInd = 0; predInd < NUM_PREDATORS; predInd++)
      distances[predInd][chosenDestInd] = -1;
  }

  for (int i = 0; i < NUM_DESTS; i++) {
    if (getDistanceToPoint(dims,obs.preyPos(),obs.positions[i+1]) == 1)
      destAssignments[i] = obs.positions[i+1]; // +1 because prey is 0
  }
}
