/*
File: PredatorStudentCpp.cpp
Author: Samuel Barrett
Description: wrapper around the student's c++ predator for assignment 1
Created:  2011-09-14
Modified: 2011-11-04
*/

#include "PredatorStudentCpp.h"

ActionProbs convertStudentAction(int action) {
  return ActionProbs(getAction(STUDENT_MOVES_OLD[action]));
}

ActionProbs convertStudentActionNew(const MoveDistribution &action) {
  ActionProbs actionProbs;

  actionProbs[getAction(Point2D( 0, 0))] = action.probNoop;
  actionProbs[getAction(Point2D(-1, 0))] = action.probLeft;
  actionProbs[getAction(Point2D( 1, 0))] = action.probRight;
  actionProbs[getAction(Point2D( 0,-1))] = action.probUp;
  actionProbs[getAction(Point2D( 0, 1))] = action.probDown;
  return actionProbs;
}

PredatorStudentCpp::PredatorStudentCpp(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd):
  Agent(rng,dims),
  name(name)
{
  createPredator(name,predatorInd);
}

PredatorStudentCpp::~PredatorStudentCpp() {
}

ActionProbs PredatorStudentCpp::step(const Observation &obs) {
  int pos[2];
  int preyPositions[1][2];
  int predatorPositions[4][2];
  
  pos[0] = obs.myPos().x;
  pos[1] = obs.myPos().y;
  preyPositions[0][0] = obs.preyPos().x;
  preyPositions[0][1] = obs.preyPos().y;
  int ind = 0;
  for (unsigned int i = 0; i < 4; i++) {
    if (ind == obs.preyInd)
      ind++;
    predatorPositions[i][0] = obs.positions[ind].x;
    predatorPositions[i][1] = obs.positions[ind].y;
    ind++;
  }
  // set the random seed for the agents
  srand(rng->randomUInt());
  if (predator.get() != NULL) {
    int action = predator->step(pos,preyPositions,predatorPositions);
    return convertStudentAction(action);
  } else {
    MoveDistribution move = predatorNew->step(pos,preyPositions[0],predatorPositions);
    return convertStudentActionNew(move);
  }
}

void PredatorStudentCpp::restart() {
}

std::string PredatorStudentCpp::generateDescription() {
  return "PredatorStudentCpp: wrapper around " + name + "'s c++ predator";
}
