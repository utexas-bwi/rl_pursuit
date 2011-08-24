#include "Common.h"
#include <cassert>
#include <cstdlib>
#include <iostream>

Point2D movePosition(const Point2D &dims, Point2D pos, Action::Type action) {
  return movePosition(dims,pos,Action::MOVES[action]);
}

Point2D movePosition(const Point2D &dims, Point2D pos, const Point2D &move) {
  if ((pos.x == 0) && (move.x < 0))
    pos.x += dims.x;
  if ((pos.y == 0) && (move.y < 0))
    pos.y += dims.y;
  pos += move;

  pos.x %= dims.x;
  pos.y %= dims.y;
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
