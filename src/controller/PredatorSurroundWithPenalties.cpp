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
const int PredatorSurroundWithPenalties::violationHistorySize = 5;

PredatorSurroundWithPenalties::PredatorSurroundWithPenalties(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims),
  planner(dims),
  captureMode(false),
  penaltyOn(false)
{
}

ActionProbs PredatorSurroundWithPenalties::step(const Observation &obs) {
  //std::cout << obs << std::endl;
  avoidLocations = obs.positions; // reset the avoid locations to the current positions of agents
  setCaptureMode(obs);
  //if (captureMode)
    //std::cout << "CAPTURE MODE ENGAGED" << std::endl;
  //else
    //std::cout << "DO NOT CAPTURE" << std::endl;

  Point2D desiredPosition = getDesiredPosition(obs);
  //std::cout << "DESIRED POSITION: " << desiredPosition << std::endl;
  //std::cout << "avoidLocations: ";
  //for (unsigned int i = 0; i < avoidLocations.size(); i++)
    //std::cout << avoidLocations[i] << " ";
  //std::cout << std::endl;
  Point2D diff;
  if (obs.myPos() == desiredPosition) {
    diff = Point2D(0,0);
  } else {
    planner.plan(obs.myPos(),desiredPosition,avoidLocations);
    if (!planner.foundPath()) {
      //std::cout << "NO PATH FOUND, moving randomly: " << obs << " " << dest << std::endl;
      return ActionProbs(Action::RANDOM);
    }
    diff = getDifferenceToPoint(dims,obs.myPos(),planner.getFirstStep());
  }
  
  prevObs = obs;
  return ActionProbs(getAction(diff));
}

Point2D PredatorSurroundWithPenalties::getDesiredPosition(const Observation &obs) {
  assert(obs.preyInd == 0);

  if (captureMode) {
    return getGreedyDesiredPosition(dims,obs);
  } else {
    // don't get too close to the prey
    for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) 
      avoidLocations.push_back(movePosition(dims,obs.preyPos(),(Action::Type)i));
    // get the desired destination
    assignTeammateAwareDesiredDests(dims,obs,destAssignments,false,false,2);
    //std::cout << "destAssignments: ";
    //for (unsigned int i = 0; i < NUM_DESTS; i++)
      //std::cout << destAssignments[i] << " ";
    //std::cout << std::endl;
    return destAssignments[obs.myInd-1]; // -1 because prey is 1
  }  
}

void PredatorSurroundWithPenalties::restart() {
  penaltyOn = false;
  captureMode = false;
  while (!violationHistory.empty())
    violationHistory.pop();
}

std::string PredatorSurroundWithPenalties::generateDescription() {
  return "PredatorSurroundWithPenalties: surrounds the prey at a distance before trying to capture it and punished predators that break this behavior";
}

void PredatorSurroundWithPenalties::setCaptureMode(const Observation &obs) {
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
