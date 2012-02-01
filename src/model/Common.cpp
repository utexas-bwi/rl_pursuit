/*
File: Common.cpp
Author: Samuel Barrett
Description: some common info for the pursuit domain
Created:  2011-08-22
Modified: 2011-08-30
*/

#include "Common.h"
#include <cassert>
#include <cmath>
#include <iostream>

Action::Type getAction(const Point2D &move) {
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    if (move == Action::MOVES[i])
      return (Action::Type)i;
  std::cout << "Action::getAction: ERROR: Action not found for: " << move << std::endl;
  assert(false);
  return Action::RANDOM;
}

ActionProbs::ActionProbs() {
  reset();
}

ActionProbs::ActionProbs(Action::Type ind){
  if (ind == Action::RANDOM) {
    for (int i = 0; i < Action::NUM_MOVES; i++)
      probs[i] = 1.0 / Action::NUM_MOVES;
  } else {
    for (int i = 0; i < Action::NUM_MOVES; i++)
      probs[i] = 0;
    probs[ind] = 1.0;
  }
}

void ActionProbs::reset() {
  for (int i = 0; i < Action::NUM_MOVES; i++)
    probs[i] = 0;
}

float& ActionProbs::operator[](Action::Type ind) {
  return probs[ind];
}

const float& ActionProbs::operator[](Action::Type ind) const {
  return probs[ind];
}

Action::Type ActionProbs::selectAction(boost::shared_ptr<RNG> rng) {
  float x = rng->randomFloat();
  float total = 0;
  for (unsigned int i = 0; i < Action::NUM_MOVES; i++) {
    total += probs[i];
    if (x < total)
      return (Action::Type)i;
  }
  return (Action::Type)(Action::NUM_MOVES-1);
}

bool ActionProbs::checkTotal() {
  float total = 0;
  for (unsigned int i = 0; i < Action::NUM_MOVES; i++)
    total += probs[i];
  return fabs(1.0 - total) < 1e-3;
}

Action::Type ActionProbs::maxAction() {
  int maxAction = 0;
  float maxVal = -1;
  for (int i = 0; i < Action::NUM_MOVES; i++) {
    if (probs[i] > maxVal) {
      maxAction = i;
      maxVal = probs[i];
    }
  }
  return (Action::Type)maxAction;
}

std::ostream& operator<<(std::ostream &out, const ActionProbs &action) {
  out << "ActionProbs(";
  for (int i = 0; i < Action::NUM_MOVES; i++) {
    if (i != 0)
      out << ",";
    out << action.probs[i];
  }
  out << ")";
  return out;
}

Point2D wrapPoint(const Point2D &dims, Point2D pos) {
  while (pos.x > dims.x)
    pos.x -= dims.x;
  while (pos.x < 0)
    pos.x += dims.x;
  
  while (pos.y > dims.y)
    pos.y -= dims.y;
  while (pos.y < 0)
    pos.y += dims.y;
  return pos;
}

Point2D movePosition(const Point2D &dims, Point2D pos, Action::Type action) {
  return movePosition(dims,pos,Action::MOVES[action]);
}

Point2D movePosition(const Point2D &dims, Point2D pos, const Point2D &move) {
  //if ((pos.x == 0) && (move.x < 0))
    //pos.x += dims.x;
  //if ((pos.y == 0) && (move.y < 0))
    //pos.y += dims.y;
  pos += move;

  pos.x %= dims.x;
  pos.y %= dims.y;
  if (pos.x < 0)
    pos.x += dims.x;
  if (pos.y < 0)
    pos.y += dims.y;
  return pos;
}

unsigned int getDistanceToPoint(const Point2D &dims, const Point2D &pos1, const Point2D &pos2) {
  Point2D delta = getDifferenceToPoint(dims,pos1,pos2);
  //std::cout << "delta: " << delta << std::endl;
  return abs(delta.x) + abs(delta.y);
}

Point2D getDifferenceToPoint(const Point2D &dims, const Point2D &start, const Point2D &end) {
  Point2D delta;

  float maxDx = 0.5 * dims.x;
  delta.x = end.x - start.x;
  if (delta.x > maxDx)
    delta.x -= dims.x;
  if (delta.x < -maxDx)
    delta.x += dims.x;
  
  float maxDy = 0.5 * dims.y;
  delta.y = end.y - start.y;
  if (delta.y > maxDy)
    delta.y -= dims.y;
  if (delta.y < -maxDy)
    delta.y += dims.y;
  return delta;
}

std::ostream& operator<<(std::ostream &out, const Observation &obs) {
  out << "<Obs: ";
  for (unsigned int i = 0; i < obs.positions.size(); i++)
    out << obs.positions[i] << " ";
  out << "Prey: " << obs.preyInd << " me: " << obs.myInd;
  out << " absPrey: "  << obs.absPrey;
  out << ">";
  return out;
}

const Point2D& Observation::preyPos() const{
  assert(preyInd >= 0);
  return positions[preyInd];
}

const Point2D& Observation::myPos() const{
  return positions[myInd];
}

int Observation::getCollision(const Point2D &pos) const{
  for (int i = 0; i < (int)positions.size(); i++) {
    if (pos == positions[i])
      return i;
  }
  return -1;
}

bool Observation::operator==(const Observation &other) const {
  if (positions.size() != other.positions.size())
    return false;
  for (unsigned int i = 0; i < positions.size(); i++) {
    if (positions[i] != other.positions[i])
      return false;
  }
  if (preyInd != other.preyInd)
    return false;
  if (myInd != other.myInd)
    return false;
  if (absPrey != other.absPrey)
    return false;
  return true;
}

void Observation::centerPrey(const Point2D &dims) {
  Point2D offset = 0.5f * dims - absPrey;
  for (unsigned int i = 0; i < positions.size(); i++)
    positions[i] = movePosition(dims,positions[i],offset);
}

void Observation::uncenterPrey(const Point2D &dims) {
  if (absPrey == preyPos())
    return;
  Point2D offset = absPrey - preyPos();
  for (unsigned int i = 0; i < positions.size(); i++)
    positions[i] = movePosition(dims,positions[i],offset);
}
