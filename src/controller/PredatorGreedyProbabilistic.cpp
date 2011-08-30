/*
File: PredatorGreedyProbabilistic.cpp
Author: Samuel Barrett
Description: implementation of a predator that's greedy with some randomness
Created:  2011-08-30
Modified: 2011-08-30
*/

#include "PredatorGreedyProbabilistic.h"

const unsigned int PredatorGreedyProbabilistic::blockedPenalty = 3;

PredatorGreedyProbabilistic::PredatorGreedyProbabilistic(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims)
{}

Action::Type PredatorGreedyProbabilistic::step(const Observation &obs) {
  Point2D desiredPosition = getGreedyDesiredPosition(dims,obs);
  // can select either dimension and go in either direction for that dimension
  Point2D dists[2]; // dir is index
  Point2D minDists;
  //std::cout << "desired: " << desiredPosition << " current: " << obs.myPos() << std::endl;
  dists[0] = wrapPoint(dims,desiredPosition - obs.myPos());
  dists[1] = wrapPoint(dims,obs.myPos() - desiredPosition);
  //std::cout << "dists: " << dists[0] << " " << dists[1] << std::endl;
  minDists.x = min(dists[0].x,dists[1].x);
  minDists.y = min(dists[0].y,dists[1].y);
  //std::cout << "minDists: " << minDists << std::endl;
  // adjust the distances to account for obstacles
  Point2D move;
  Point2D pos;
  int collision;
  //std::cout << dists[0] << " " << dists[1] << std::endl;
  for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
    move = Action::MOVES[i];
    pos = movePosition(dims,obs.myPos(),move);
    collision = obs.getCollision(pos);
    //std::cout << move << " " << collision << std::endl;
    if ((collision >= 0) && (collision != obs.preyInd)) {
      if (move.x > 0)
        dists[0].x += blockedPenalty;
      else if (move.x < 0)
        dists[1].x += blockedPenalty;
      else if (move.y > 0)
        dists[0].y += blockedPenalty;
      else if (move.y < 0)
        dists[1].y += blockedPenalty;
    }
  }
  float probMinX = softmax(minDists.x,minDists.y,2); // +2 -> prefer larger dimension
  //std::cout << "probMinX: " << probMinX << std::endl;
  if (rng->randomFloat() < probMinX) {
    float prob = softmax(dists[0].x,dists[1].x,-2); // -2 -> prefer shorter direction
    if (rng->randomFloat() < prob)
      return Action::RIGHT;
    else
      return Action::LEFT;
  } else {
    float prob = softmax(dists[0].y,dists[1].y,-2); // -2 -> prefer shorter direction
    if (rng->randomFloat() < prob)
      return Action::UP;
    else
      return Action::DOWN;
  }
}

void PredatorGreedyProbabilistic::restart() {
}

std::string PredatorGreedyProbabilistic::generateDescription() {
  return "PredatorGreeyProbabilistic: a predator that's greedy with some randomness";
}
