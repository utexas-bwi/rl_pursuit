/*
File: PredatorTeammateAware.cpp
Author: Samuel Barrett
Description: a teammate aware predator - lets the farthest away predators select their destination first, then runs A* to reach the destination
Created:  2011-08-31
Modified: 2011-08-31
*/

#include "PredatorTeammateAware.h"
#include <cassert>

PredatorTeammateAware::PredatorTeammateAware(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims),
  planner(dims)
{
}

void PredatorTeammateAware::restart() {
}

std::string PredatorTeammateAware::generateDescription() {
  return "PredatorTeammateAware: lets the farthest predator select the dests first, then uses A* to plan the path";
}

ActionProbs PredatorTeammateAware::step(const Observation &obs) {
  Point2D dest = getTeammateAwareDesiredPosition(dims,obs);
  planner.plan(obs.myPos(),dest,obs.positions);
  if (!planner.foundPath()) {
    //std::cout << "NO PATH FOUND, moving randomly: " << obs << " " << dest << std::endl;
    return ActionProbs(Action::RANDOM);
  }
  Point2D diff = getDifferenceToPoint(dims,obs.myPos(),planner.getFirstStep());
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    if (diff == Action::MOVES[i])
      return ActionProbs((Action::Type)i);
  std::cout << "Action not found for: " << diff << std::endl;
  return ActionProbs(Action::RANDOM);
}

Point2D getTeammateAwareDesiredPosition(const Point2D &dims, const Observation &obs) {
  // check how far each predator is to each surrounding spot
  // FIXME assuming prey is in position 0
  assert(obs.preyInd == 0);
  // FIXME assuming 4 predators
  const unsigned int NUM_PREDATORS = 4;
  int distances[NUM_PREDATORS][Action::NUM_NEIGHBORS];
  Point2D neighbors[Action::NUM_NEIGHBORS];
  //int positionAssignments[NUM_PREDATORS];
  //for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++)
    //positionAssignments[pred] = -1;
  int minDists[NUM_PREDATORS];
  unsigned int minInds[NUM_PREDATORS];
  for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++)
    minDists[pred] = 999999;
  

  Point2D dest;
  for (unsigned int neighbor = 0; neighbor < Action::NUM_NEIGHBORS; neighbor++) {
    dest = movePosition(dims,obs.preyPos(),(Action::Type)neighbor);
    neighbors[neighbor] = dest;
    for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++) {
      distances[pred][neighbor] = getDistanceToPoint(dims,obs.positions[pred + 1],dest); // +1 because prey is in position 0
      if (distances[pred][neighbor] < minDists[pred]) {
        minDists[pred] = distances[pred][neighbor];
        minInds[pred] = neighbor;
      }
    }
  }
  
  int maxDist;
  unsigned int maxDistPred = 0;
  unsigned int chosenNeighbor = 0;
  //std::cout << obs << std::endl;
  while(true) {
    // get which predator is the farthest from the points
    //std::cout << "minDists: ";
    maxDist = -1;
    for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++) {
      //std::cout << minDists[pred] << " ";
      if (minDists[pred] > maxDist) {
        maxDist = minDists[pred];
        maxDistPred = pred;
      }
    }
    //std::cout << std::endl;

    chosenNeighbor = minInds[maxDistPred];
    //std::cout << "Predator " << maxDistPred + 1<< " gets neighbor " << chosenNeighbor << std::endl;
    //std::cout << "  with dists: ";
    //for (unsigned int neighbor = 0; neighbor < Action::NUM_NEIGHBORS; neighbor++)
      //std::cout << distances[maxDistPred][neighbor] << " ";
    //std::cout << std::endl;

    if (maxDistPred + 1 == obs.myInd) { // +1 because prey is 0
      // if this is us, just return the chosen position
      // unless we're at that position, then move onto the prey
      if (neighbors[chosenNeighbor] == obs.myPos())
        return obs.preyPos();
      else
        return neighbors[chosenNeighbor];
    }
    // make it clear this predator has chosen
    minDists[maxDistPred] = -1;
    // remove this option for the other predators
    for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++) {
      if (minDists[pred] < 0)
        continue;
      distances[pred][chosenNeighbor] = 999999;
      if (minInds[pred] == chosenNeighbor) {
        minDists[pred] = 999999;
        for (unsigned int neighbor = 0; neighbor < Action::NUM_NEIGHBORS; neighbor++) {
          if (distances[pred][neighbor] < minDists[pred]) {
            minDists[pred] = distances[pred][neighbor];
            minInds[pred] = neighbor;
          }
        }
      }
    } // end for pred
  } // end while
}
