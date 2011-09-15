/*
File: PredatorStudentCpp.cpp
Author: Samuel Barrett
Description: wrapper around the student's c++ predator for assignment 1
Created:  2011-09-14
Modified: 2011-09-14
*/

#include "PredatorStudentCpp.h"

PredatorStudentCpp::PredatorStudentCpp(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int):
  Agent(rng,dims),
  name(name)
{
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

  int action = predator->step(pos,preyPositions,predatorPositions);
  return ActionProbs((Action::Type)action);
}

void PredatorStudentCpp::restart() {
}

std::string PredatorStudentCpp::generateDescription() {
  return "PredatorStudentCpp: wrapper around " + name + "'s c++ predator";
}
