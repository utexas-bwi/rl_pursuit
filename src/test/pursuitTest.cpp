/*
File: pursuitTest.cpp
Author: Samuel Barrett
Description: a test of the pursuit domain all together
Created:  2011-08-22
Modified: 2011-08-22
*/

#include <iostream>
#include <rl_pursuit/model/WorldModel.h>
#include <rl_pursuit/model/AgentModel.h>
#include <rl_pursuit/controller/World.h>
#include <rl_pursuit/common/RNG.h>
#include <rl_pursuit/common/Util.h>
#include <rl_pursuit/controller/AgentFactory.h>

//boost::shared_ptr<Agent> genMCTSPred(const Point2D &dims) {
  //UCTEstimator<Observation> *valueEstimator = new UCTEstimator<Observation>(rng,Actions::NUM_ACTIONS,0.8,0.9,1.0,0,0,0,9999);
  //MCTS<Observation,Action::Type> *planner = new MCTS<Observation,Action::Type>(model,valueEstimator,1000,0,dims.x + dims.y * 2);
  //return new PredatorMCTS(new RNG(4),dims,planner);
//}

int main() {
  const Point2D dims(5,5);
  WorldModel worldModel(dims);
  World worldController(boost::shared_ptr<RNG>(new RNG(0)),worldModel);

  worldController.addAgent(AgentModel(0,0,PREY),createAgent(0,dims,"prey"));
  for (int i = 0; i < 4; i++)
    worldController.addAgent(AgentModel(0,0,PREDATOR),createAgent(i+1,dims,"greedy"),true);

  //worldController.addAgent(AgentModel(0,0,ADHOC),genMCTSPred(dims),false);

  double startTime = Util::getTime();
  float avgSteps = 0;
  for (int run = 0; run < 1; run++) {
    worldController.randomizePositions();

    Observation obs;
    worldModel.generateObservation(obs);
    std::cout << obs << std::endl;
    unsigned int numSteps = 0;
    while (!worldModel.isPreyCaptured()) {
    //for (int i = 0; i < 2; i++) {
      //std::cout << "STEP" << std::endl;
      numSteps += 1;
      worldController.step();
      worldModel.generateObservation(obs);
      std::cout << obs << std::endl;
    }
    std::cout << numSteps << std::endl;
    avgSteps = (avgSteps * run + numSteps) / (run + 1);
  }
  double endTime = Util::getTime();
  std::cout << "AVG: " << avgSteps << std::endl;
  std::cout << "time: " << endTime - startTime << std::endl;

  return 0;
}
