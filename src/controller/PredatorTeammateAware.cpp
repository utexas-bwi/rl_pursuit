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
  counter = 0;
}

void PredatorTeammateAware::restart() {
}

std::string PredatorTeammateAware::generateDescription() {
  return "PredatorTeammateAware: lets the farthest predator select the dests first, then uses A* to plan the path";
}

ActionProbs PredatorTeammateAware::step(const Observation &obs) {
  counter++;
  //std::cout << "TA: " << counter << " " << obs << std::endl;
  Point2D dest = getTeammateAwareDesiredPosition(dims,obs);
  planner.plan(obs.myPos(),dest,obs.positions);
  if (!planner.foundPath()) {
    //std::cout << "NO PATH FOUND, moving randomly: " << obs << " " << dest << std::endl;
    return ActionProbs(Action::RANDOM);
  }
  Point2D diff = getDifferenceToPoint(dims,obs.myPos(),planner.getFirstStep());
  return ActionProbs(getAction(diff));
}

Point2D getTeammateAwareDesiredPosition(const Point2D &dims, const Observation &obs) {
  Point2D dests[NUM_PREDATORS];
  assignTeammateAwareDesiredDests(dims,obs,dests,true,true,1);
  //std::cout << obs << std::endl;
  //std::cout << "MY DEST: " << dests[obs.myInd + 1] << std::endl;
  //std::cout << "DESTS: " << dests[0] << " " << dests[1] << " " << dests[2] << " " << dests[3] << std::endl;
  return dests[obs.myInd - 1]; // -1 because prey is 0
}

void assignTeammateAwareDesiredDests(const Point2D &dims, const Observation &obs, Point2D dests[NUM_PREDATORS], bool stopAfterAssigningCurrentPred, bool moveOntoPreyIfAtDest, int distFactor) {
  // FIXME assuming prey is in position 0
  assert(obs.preyInd == 0);
  // FIXME assuming 4 predators and 1 prey
  assert(obs.positions.size() == NUM_PREDATORS + 1);

  // check how far each predator is to each surrounding spot
  int distances[NUM_PREDATORS][NUM_DESTS];
  Point2D possibleDests[NUM_DESTS];
  int minDists[NUM_PREDATORS];
  unsigned int minInds[NUM_PREDATORS];
  for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++)
    minDists[pred] = 999999;
  

  Point2D dest;
  for (unsigned int destInd = 0; destInd < NUM_DESTS; destInd++) {
    dest = movePosition(dims,obs.preyPos(),distFactor * Action::MOVES[destInd]);
    possibleDests[destInd] = dest;
    for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++) {
      distances[pred][destInd] = getDistanceToPoint(dims,obs.positions[pred + 1],dest); // +1 because prey is in position 0
      if (distances[pred][destInd] < minDists[pred]) {
        minDists[pred] = distances[pred][destInd];
        minInds[pred] = destInd;
      }
    }
  }
  
  int maxDist;
  unsigned int maxDistPred = 0;
  unsigned int chosenDest = 0;
  //std::cout << obs << std::endl;
  //while(true) {
  for (int numUnassignedPreds = NUM_PREDATORS; numUnassignedPreds > 0; numUnassignedPreds--) {
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
    //std::cout << "maxDistPred: " << maxDistPred << std::endl;

    chosenDest = minInds[maxDistPred];
    //std::cout << "chosenDest: " << chosenDest << " " << possibleDests[chosenDest] << std::endl;
    dests[maxDistPred] = possibleDests[chosenDest];
    // if the predator is already at that pos, move onto the prey
    if (moveOntoPreyIfAtDest && (dests[maxDistPred] == obs.positions[maxDistPred + 1]))
      dests[maxDistPred] = obs.preyPos();
    if (stopAfterAssigningCurrentPred && (maxDistPred + 1 == obs.myInd )) // +1 because prey is 0
      return;
    //std::cout << "Predator " << maxDistPred + 1<< " gets neighbor " << chosenDest << std::endl;
    //std::cout << "  with dists: ";
    //for (unsigned int neighbor = 0; neighbor < NUM_DESTS; neighbor++)
      //std::cout << distances[maxDistPred][neighbor] << " ";
    //std::cout << std::endl;
    
    // make it clear this predator has chosen
    minDists[maxDistPred] = -1;
    // remove this option for the other predators
    for (unsigned int pred = 0; pred < NUM_PREDATORS; pred++) {
      if (minDists[pred] < 0)
        continue;
      distances[pred][chosenDest] = 999999;
      if (minInds[pred] == chosenDest) {
        minDists[pred] = 999999;
        for (unsigned int neighbor = 0; neighbor < NUM_DESTS; neighbor++) {
          if (distances[pred][neighbor] < minDists[pred]) {
            minDists[pred] = distances[pred][neighbor];
            minInds[pred] = neighbor;
          }
        }
      }
    } // end for pred
  } // end while
}
