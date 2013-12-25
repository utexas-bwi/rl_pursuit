#include <iostream>
#include "ToyModel.h"
#include <rl_pursuit/planning/UCTEstimator.h>
#include <rl_pursuit/planning/MCTS.h>
#include <rl_pursuit/common/Util.h>

int main()
{
  double time = Util::getTime();
  unsigned int size = 10;
  ToyModel *planModel = new ToyModel(size);
  UCTEstimator<State> *estimator = new UCTEstimator<State>(new RNG(5),planModel->numActions(),0.8,0.95,1.0,0,0,0,BIGNUM);
  MCTS<State,Action> planner(planModel,estimator,1000,0,20);

  ToyModel world(size);
  int state = world.getState();
  bool terminal = false;
  float reward = 0;
  unsigned int action;
  unsigned int counter = 0;

  while (!terminal) {
    planner.search(state);
    action = planner.selectWorldAction(state);
    //std::cout << state << " " << action << std::endl;
    world.takeAction(action,reward,state,terminal);
    counter++;
  }
  //std::cout << state << std::endl;
  std::cout << Util::getTime() - time << std::endl;
  std::cout << "numSteps:" <<  counter << std::endl;
  return 0;
}
