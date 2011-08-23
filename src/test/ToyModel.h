#ifndef TOYMODEL_ZY52NKWE
#define TOYMODEL_ZY52NKWE

/*
File: ToyModel.h
Author: Samuel Barrett
Description: a toy model for testing the planning
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <planning/Model.h>

typedef int State;
typedef unsigned int Action;

class ToyModel: public Model<State, Action> {
public:
  ToyModel (unsigned int size):
    size(size),
    state(0)
  {}

  Action numActions() {
    return 3;
  }

  void setState(const State &nstate) {
    state = nstate;
  }

  State getState() {
    return state;
  }

  void takeAction(const Action &action, float &reward, State &state, bool &terminal) {
    //std::cout << "takeAction(" << state << "," << action << ") --> ";
    int direction = 2 * (state % 2) - 1;
    switch (action) {
      case 0:
        state -= direction;
        break;
      case 2:
        state += direction;
        break;
      default:
        break;
    }

    terminal = false;
    reward = 0;
    if (state < 0) {
      state = 0;
      reward = -1;
    } else if ((unsigned int)state >= size) {
      reward = 1;
      terminal = true;
    }
    //std::cout << std::boolalpha << state << "," << reward << "," << terminal << std::endl;
  }

private:
  unsigned int size;
  State state;
};

#endif /* end of include guard: TOYMODEL_ZY52NKWE */
