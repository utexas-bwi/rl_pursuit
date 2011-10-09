/*
File: PreyAvoidNeighbor.cpp
Author: Samuel Barrett
Description: a prey that avoids neighboring predators and noops or moves randomly otherwise
Created:  2011-09-30
Modified: 2011-09-30
*/

#include "PreyAvoidNeighbor.h"
#include <boost/lexical_cast.hpp>

const float PreyAvoidNeighbor::noopWeight = 0.25;

PreyAvoidNeighbor::PreyAvoidNeighbor(boost::shared_ptr<RNG> rng, const Point2D &dims):
  Agent(rng,dims)
{
}

ActionProbs PreyAvoidNeighbor::step(const Observation &obs) {
  std::vector<Point2D> neighborMoves;
  getNeighborMoves(obs,neighborMoves);

  if (neighborMoves.size() == 0)
    return moveWithNoNeighbors();

  // try to move away from a neighboring predator
  std::vector<Point2D> desiredMoves;
  bool occupied;
  for (unsigned int i = 0; i < neighborMoves.size(); i++) {
    occupied = false;
    for (unsigned int j = 0; j < neighborMoves.size(); j++) {
      if (-1 * neighborMoves[i] == neighborMoves[j])
        occupied = true;
    }
    if (!occupied)
      desiredMoves.push_back(-1 * neighborMoves[i]);
  }

  if (desiredMoves.size() == 0) {
    // the desired moves are blocked, i.e. there's a predator on either side
    // check for any open spot
    for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
      occupied = false;
      for (unsigned int j = 0; j < neighborMoves.size(); j++) {
        if (Action::MOVES[i] == neighborMoves[j])
          occupied = true;
      }
      if (!occupied)
        desiredMoves.push_back(Action::MOVES[i]);
    }
  }
  assert(desiredMoves.size() != 0);
  float weight = 1.0 / desiredMoves.size();
  ActionProbs action;
  for (unsigned int i = 0; i < desiredMoves.size(); i++)
    action[getAction(desiredMoves[i])] = weight;
  return action;
}

void PreyAvoidNeighbor::restart() {
}

std::string PreyAvoidNeighbor::generateDescription() {
  return std::string("PreyAvoidNeighbor: avoids neighboring predators, if no neighbors, stays in place (") + boost::lexical_cast<std::string>(noopWeight) + ") or moves randomly (" + boost::lexical_cast<std::string>(1-noopWeight) + ")";
}
  
void PreyAvoidNeighbor::getNeighborMoves(const Observation &obs, std::vector<Point2D> &neighborMoves) {
  neighborMoves.clear();

  Point2D pos;
  bool occupied;
  for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
    pos = movePosition(dims,obs.myPos(),(Action::Type)i);
    occupied = false;
    for (unsigned int j = 0; j < obs.positions.size(); j++) {
      if (obs.positions[j] == pos) {
        occupied = true;
        break;
      }
    }
    if (occupied)
      neighborMoves.push_back(Action::MOVES[i]);
  }
}

ActionProbs PreyAvoidNeighbor::moveWithNoNeighbors() {
  ActionProbs action;
  action[Action::NOOP] = 0.75;
  action[Action::RIGHT] = 0.1875;
  action[Action::LEFT] = 0.0625;
  action[Action::UP] = 0.0;
  action[Action::DOWN] = 0.0;
  //float weight = (1 - noopWeight) / (Action::NUM_ACTIONS - 1);
  //for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++) {
    //if (i == Action::NOOP)
      //action[(Action::Type)i] = noopWeight;
    //else
      //action[(Action::Type)i] = weight;
  //}
  return action;
}
