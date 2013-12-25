#include "PredatorGreedy.h"
#include <cmath>
#include <cassert>
#include <rl_pursuit/common/Util.h>
#include <iostream>

Point2D getGreedyDesiredPosition(const Point2D &dims, const Observation &obs) {
  unsigned int minDist = -1;
  unsigned int dist;
  Point2D minPos;
  Point2D pos;
  const Point2D &preyPos = obs.preyPos();
  const Point2D &myPos = obs.myPos();
  //std::cout << "me: " << myPos << " prey: " << preyPos << std::endl;
  int collision;

  for (int action = 0; action < Action::NUM_NEIGHBORS; action++) {
    pos = movePosition(dims,preyPos,(Action::Type)action);
    //std::cout << "pos: " << pos << std::endl;
    collision = obs.getCollision(pos);
    if (collision == (int)obs.myInd) {
      // already beside the prey, try to move onto it in case it moves
      return preyPos;
    } else if (collision < 0) {
      // not already occupied by another predator
      dist = getDistanceToPoint(dims,pos,myPos);
      //std::cout << "dist: " << dist << std::endl;
      if (dist < minDist) {
        minDist = dist;
        minPos = pos;
      }
    }
  }

  return minPos;
}

Action::Type greedyObstacleAvoid(const Point2D &dims, const Observation &obs, const Point2D &dest) {
  const Point2D &myPos = obs.myPos();
  //std::cout << "me: " << myPos << " dest: " << dest << std::endl;
  Point2D diff = getDifferenceToPoint(dims,myPos,dest);
  //std::cout << "diff: " << diff << std::endl;

  Point2D move1 = Point2D(sgn(diff.x),0);
  Point2D move2 = Point2D(0,sgn(diff.y));
  //std::cout << "moves: " << move1 << " " << move2 << std::endl;
  Point2D pos1 = movePosition(dims,myPos,move1);
  Point2D pos2 = movePosition(dims,myPos,move2);
  //std::cout << "pos: " << pos1 << " " << pos2 << std::endl;
  int col1 = obs.getCollision(pos1);
  int col2 = obs.getCollision(pos2);
  bool pos1Blocked = ((col1 >= 0) && (col1 != obs.preyInd));
  bool pos2Blocked = ((col2 >= 0) && (col2 != obs.preyInd));

  Point2D move;

  if (pos1Blocked && pos2Blocked) {
    // choose randomly
    return Action::RANDOM;
  } else if (pos1Blocked && !pos2Blocked && (move2 != Action::MOVES[Action::NOOP])) {
    // if 1 is blocked, choose 2 unless it is a noop
    move = move2;
  } else if (pos2Blocked && !pos1Blocked && (move1 != Action::MOVES[Action::NOOP])) {
    // if 2 is blocked, choose 1 unless it is a noop
    move = move1;
  } else {
    // if not blocked, minimize the larger dimension first
    if (abs(diff.x) > abs(diff.y))
      move = move1;
    else
      move = move2;
  }

  for (int i = 0; i < Action::NUM_ACTIONS; i++) {
    if (move == Action::MOVES[i])
      return (Action::Type)i;
  }
  assert(false); // should never happen
  return Action::RANDOM;
}

ActionProbs PredatorGreedy::step(const Observation &obs) {
  Point2D dest = getGreedyDesiredPosition(dims,obs);
  Action::Type action = greedyObstacleAvoid(dims,obs,dest);
  return ActionProbs(action);
}
