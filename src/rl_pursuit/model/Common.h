#ifndef COMMON_B97S1VDF
#define COMMON_B97S1VDF

/*
File: Common.h
Author: Samuel Barrett
Description: some common information
Created:  2011-08-22
Modified: 2011-08-30
*/

#include <vector>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include <rl_pursuit/common/Point2D.h>
#include <rl_pursuit/common/RNG.h>

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

namespace Action {
  enum Type { // order must match moves below
    RIGHT,
    LEFT,
    UP,
    DOWN,
    NOOP,
    NUM_MOVES,
    RANDOM,
    NUM_NEIGHBORS = NOOP,
    NUM_ACTIONS = NUM_MOVES
  };

  static const Point2D VARIABLE_IS_NOT_USED MOVES[NUM_MOVES] = {Point2D(1,0),Point2D(-1,0),Point2D(0,1),Point2D(0,-1),Point2D(0,0)};
}

Action::Type getAction(const Point2D &move);

class ActionProbs {
public:
  ActionProbs();
  explicit ActionProbs(Action::Type ind);
  void reset();
  float& operator[](Action::Type ind);
  const float& operator[](Action::Type ind) const;
  Action::Type selectAction(boost::shared_ptr<RNG> rng);
  bool checkTotal();
  Action::Type maxAction();
  float overlap(const ActionProbs &other) const;

private:
  float probs[Action::NUM_MOVES];
  friend std::ostream& operator<<(std::ostream &out, const ActionProbs &action);
};

Point2D wrapPoint(const Point2D &dims, Point2D pos);
Point2D movePosition(const Point2D &dims, Point2D pos, Action::Type action);
Point2D movePosition(const Point2D &dims, Point2D pos, const Point2D &move);
unsigned int getDistanceToPoint(const Point2D &dims, const Point2D &pos1, const Point2D &pos2);
Point2D getDifferenceToPoint(const Point2D &dims, const Point2D &start, const Point2D &end);

struct Observation {
  Observation();
  std::vector<Point2D> positions;
  int preyInd;
  unsigned int myInd;
  Point2D absPrey;
  bool prevPreyCaptured;

  const Point2D& preyPos() const;
  const Point2D& myPos() const;
  int getCollision(const Point2D &pos) const;
  bool operator==(const Observation &other) const;

  void centerPrey(const Point2D &dims);
  void uncenterPrey(const Point2D &dims);
  //bool isPreyCaptured(const Point2D &dims) const;
  bool didPreyMoveIllegally(const Point2D &dims, const Point2D &prevAbsPrey);
};

std::ostream& operator<<(std::ostream &out, const Observation &obs) ;

#endif /* end of include guard: COMMON_B97S1VDF */
