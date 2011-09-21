/*
File: State.h
Author: Samuel Barrett
Description: State for planning
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "State.h"

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

State_t getStateFromObs(const Point2D &dims, const Observation &obs) {
  State_t state = 0;
  for (int i = ((int)obs.positions.size())-1; i >= 0; i--) {
    state *= dims.y;
    state += obs.positions[i].x;
    state *= dims.x;
    state += obs.positions[i].y;
  }
  return state;
}

void getPositionsFromState(State_t state, const Point2D &dims, std::vector<Point2D> &positions) {
  for (unsigned int i = 0; i < positions.size(); i++) {
    positions[i].y = state % dims.y;
    state /= dims.y;
    positions[i].x = state % dims.x;
    state /= dims.x;
  }
  assert(state == 0);
}

