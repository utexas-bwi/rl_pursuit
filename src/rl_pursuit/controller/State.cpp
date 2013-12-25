/*
File: State.h
Author: Samuel Barrett
Description: State for planning
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "State.h"
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <rl_pursuit/common/Util.h>

/*
State::State(const Observation &obs) {
  for (unsigned int i = 0; i < STATE_SIZE; i++)
    positions[i] = obs.positions[i];
}

bool State::operator<(const State &other) const{
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    if (positions[i].x < other.positions[i].x)
      return true;
    else if (positions[i].x > other.positions[i].x)
      return false;
    else if (positions[i].y < other.positions[i].y)
      return true;
    else if (positions[i].y > other.positions[i].y)
      return false;
  }
  // equal
  return false;
}

bool State::operator==(const State &other) const{
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    if (positions[i].x != other.positions[i].x)
      return false;
    else if (positions[i].y != other.positions[i].y)
      return false;
  }
  // equal
  return true;
}

std::ostream& operator<<(std::ostream &out, const State &state) {
  out << "<State ";
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    out << state.positions[i];
  }
  out << ">";
  return out;
}

std::size_t hash_value(const State &s) {
  std::size_t seed = 0;
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    boost::hash_combine(seed,s.positions[i].x);
    boost::hash_combine(seed,s.positions[i].y);
  }
  return seed;
}
*/

State_t getStateFromObs(const Point2D &dims, const Observation &obs, bool usePreySymmetry) {
  State_t state = 0;
  Point2D pos;
  Point2D offset(0,0);
  int endInd = 0;

  if (usePreySymmetry) {
    // move the first agent to the center
    offset = (0.5f * dims) - obs.positions[0];
    endInd = 1;
  }

  for (int i = ((int)obs.positions.size())-1; i >= endInd; i--) {
    pos = obs.positions[i];
    pos = movePosition(dims,pos,offset);
    state *= dims.y;
    state += pos.y;
    state *= dims.x;
    state += pos.x;
  }
  return state;
}

void getPositionsFromState(State_t state, const Point2D &dims, std::vector<Point2D> &positions, bool usePreySymmetry) {
  // first agent is in center
  //positions[0] = 0.5f * dims;
  getPositionsFromState(state,dims,positions,0.5f * dims, usePreySymmetry);
}

void getPositionsFromState(State_t state, const Point2D &dims, std::vector<Point2D> &positions, const Point2D &preyPos, bool usePreySymmetry) {
  State_t origState(state);
  Point2D offset(0,0);
  int startInd = 0;
  
  if (usePreySymmetry) {
    startInd = 1;
    positions[0] = preyPos;
    offset = preyPos - 0.5f * dims;
  }
  for (unsigned int i = startInd; i < positions.size(); i++) {
    positions[i].x = state % dims.x;
    state /= dims.x;
    positions[i].y = state % dims.y;
    state /= dims.y;
    positions[i] = movePosition(dims,positions[i],offset);
  }
  if (state != 0) {
    std::cerr << "NOT ZERO: " << state << std::endl;
    std::cerr << "original state: " << origState << std::endl;
    std::cerr << "What we got: ";
    for (unsigned int i = 0; i < positions.size(); i++)
      std::cerr << positions[i] << " ";
    std::cerr << std::endl;
  }
  assert(state == 0);
}

StateConverter::StateConverter(unsigned int numBeliefs, unsigned int numBins):
  numBeliefs(numBeliefs),
  numBins(numBins)
{
  const double eps = 1e-10;
  binSize = (1.0 + eps) / numBins;
}

State_t StateConverter::convertBeliefStateToGeneralState(const State_t &state) {
  State_t generalState(state);
  for (unsigned int i = 0; i < numBeliefs; i++)
    generalState /= numBins;
  return generalState;
}

void StateConverter::convertGeneralStateToBeliefState(State_t &state, const std::vector<double> &beliefs) {
  double val;
  assert(numBeliefs == beliefs.size());
  for (unsigned int i = 0; i < numBeliefs; i++) {
    val = beliefs[i];
    //std::cout << val << "(" << discretizeProb(val) <<  ") ";
    state *= numBins;
    state += discretizeProb(val);
  }
  //std::cout << std::endl;
}

unsigned int StateConverter::discretizeProb(double prob) {
  return prob / binSize;
}

std::string StateConverter::generateDescription(unsigned int indentation) {
  std::string msg;
  msg += indent(indentation) + "# Beliefs: " + boost::lexical_cast<std::string>(numBeliefs) + "\n";
  msg += indent(indentation) + "# Bins: " + boost::lexical_cast<std::string>(numBins);
  return msg;
}
