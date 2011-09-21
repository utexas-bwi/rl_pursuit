#ifndef STATE_FVUZR054
#define STATE_FVUZR054

/*
File: State.h
Author: Samuel Barrett
Description: State for planning
Created:  2011-09-21
Modified: 2011-09-21
*/

#include <common/Point2D.h>
#include <model/Common.h>

const unsigned int STATE_SIZE = 5;
typedef uint64_t State_t;
State_t getStateFromObs(const Point2D &dims, const Observation &obs);
void getPositionsFromState(State_t state, const Point2D &dims, std::vector<Point2D> &positions);

//struct State {
  //State() {};
  //State(const Observation &obs);
  //Point2D positions[STATE_SIZE];
  //bool operator<(const State &other) const;
  //bool operator==(const State &other) const;
//};
//std::ostream& operator<<(std::ostream &out, const State &state);
//std::size_t hash_value(const State &s);

#endif /* end of include guard: STATE_FVUZR054 */
