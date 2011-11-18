/*
File: WorldBeliefMDP.cpp
Author: Samuel Barrett
Description: tests WorldBeliefMDP
Created:  2011-10-18
Modified: 2011-10-18
*/

#include <gtest/gtest.h>
#include <controller/WorldBeliefMDP.h>
#include "AgentDummyTest.h"
#include <factory/PlanningFactory.h>
#include <factory/WorldFactory.h>

class WorldBeliefMDPTest: public ::testing::Test {
public:
  WorldBeliefMDPTest():
    rng(new RNG(0)),
    updaterRNG(new RNG(1)),
    //mdpRng(new RNG(2)),
    //worldRng(new RNG(3)),
    //worldRng2(new RNG(4)),
    dims(5,5),
    stateConverter(3,5),
    beliefMDP(boost::static_pointer_cast<WorldBeliefMDP>(createWorldMDP(rng,dims,true,true,BAYESIAN_UPDATES,stateConverter,0.0,true))),
    models(3,std::vector<boost::shared_ptr<Agent> >(5,boost::shared_ptr<Agent>())),
    modelsDummy(3,std::vector<boost::shared_ptr<AgentDummyTest> >(5,boost::shared_ptr<AgentDummyTest>())),
    adhocInd(1)
  {
    model = beliefMDP->model;
    world = beliefMDP->controller;
    updater = beliefMDP->modelUpdater;
    // set up the agents
    boost::shared_ptr<AgentDummyTest> agent;
    for (int i = 0; i < 5; i++) {
      agent = boost::shared_ptr<AgentDummyTest>(new AgentDummyTest(rng,Point2D(5,5)));
      trueAgents.push_back(agent);
      agentsAbstract.push_back(agent);
    }
    std::vector<AgentModel> agentModels;
    createAgentModels(adhocInd-1,agentModels);
    for (int i = 0; i < 5; i++)
      agentModels[i].pos = Point2D(i,i);
    beliefMDP->addAgents(agentModels,agentsAbstract);
    beliefMDP->adhocAgent = trueAgents[adhocInd];
   
    // create the models
    for (int i = 0; i < 3; i++) {
      for (unsigned int j = 0; j < 5; j++) {
        if (j == adhocInd)
          agent = trueAgents[j];
        else
          agent = boost::shared_ptr<AgentDummyTest>(new AgentDummyTest(rng,Point2D(5,5)));
        models[i][j] = agent;
        modelsDummy[i][j] = agent;
      }
      modelPrior.push_back(1.0);
    }
    modelDescriptions.push_back("Correct");
    modelDescriptions.push_back("Incorrect");
    modelDescriptions.push_back("Close");

    std::cout << "HERE" << std::endl << std::flush;
    boost::shared_ptr<ModelUpdaterBayes> updater2(new ModelUpdaterBayes(updaterRNG,beliefMDP,models,modelPrior,modelDescriptions,BAYESIAN_UPDATES));
    std::cout << "HERE2" << std::endl << std::flush;
    updater->set(*updater2);
    std::cout << "HERE3" << std::endl << std::flush;
    //updater = boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(updaterRNG,mdp,models,modelPrior,modelDescriptions,BAYESIAN_UPDATES));

    //world = boost::shared_ptr<World>(new World(worldRng2,model,0.0));
    //beliefMDP = boost::shared_ptr<WorldBeliefMDP>(new WorldBeliefMDP(rng,model,world,trueAgents[adhocInd],stateConverter,updater));
  }

  void checkNumSteps(const std::vector<boost::shared_ptr<AgentDummyTest> > &agents, unsigned int numSteps) {
    for (unsigned int i = 0; i < agents.size(); i++) {
      if (i == adhocInd)
        continue;
      EXPECT_EQ(numSteps,agents[i]->numSteps);
    }
  }

  void checkModelsSteps(unsigned int numSteps) {
    for (unsigned int i = 0; i < modelsDummy.size(); i++)
      checkNumSteps(modelsDummy[i],numSteps);
  }

protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<RNG> updaterRNG;
  //boost::shared_ptr<RNG> mdpRng;
  //boost::shared_ptr<RNG> worldRng;
  //boost::shared_ptr<RNG> worldRng2;
  Point2D dims;
  boost::shared_ptr<WorldModel> model;
  boost::shared_ptr<World> world;
  //boost::shared_ptr<WorldMDP> mdp;
  std::vector<boost::shared_ptr<AgentDummyTest> > trueAgents;
  std::vector<boost::shared_ptr<Agent> > agentsAbstract;
  boost::shared_ptr<ModelUpdaterBayes> updater;
  
  StateConverter stateConverter;
  boost::shared_ptr<WorldBeliefMDP> beliefMDP;

  std::vector<std::vector<boost::shared_ptr<Agent> > > models;
  std::vector<std::vector<boost::shared_ptr<AgentDummyTest> > > modelsDummy;

  std::vector<double> modelPrior;
  std::vector<std::string> modelDescriptions;
  unsigned int adhocInd;
};

TEST_F(WorldBeliefMDPTest,GetState) {
  Observation obs;
  world->generateObservation(obs);
  State_t worldState = getStateFromObs(dims,obs,true);
  State_t worldBeliefState = beliefMDP->getState(obs);

  State_t worldState2 = stateConverter.convertBeliefStateToGeneralState(worldBeliefState);
  EXPECT_EQ(worldState,worldState2);
  State_t worldState3 = worldBeliefState;
  double beliefs[3];
  for (int i = 0; i < 3; i++) {
    beliefs[i] = worldState3 % 5;
    worldState3 /= 5;
  }
  EXPECT_EQ(worldState3,worldState2);
  for (int i = 0; i < 3; i++)
    EXPECT_EQ(1,beliefs[i]);
}

TEST_F(WorldBeliefMDPTest,SetBeliefs) {
  std::vector<double> prior;
  prior.push_back(1.0);
  prior.push_back(2.0);
  prior.push_back(0.5);
  boost::shared_ptr<ModelUpdaterBayes> updater2 = boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(updaterRNG,beliefMDP,models,prior,modelDescriptions,BAYESIAN_UPDATES));
  
  std::vector<double> b;
  b = updater->getBeliefs();
  for (unsigned int i = 0; i < 3; i++)
    EXPECT_NEAR(0.333333,b[i],0.0001);
  b = updater2->getBeliefs();
  EXPECT_NEAR(1.0 / 3.5,b[0],0.0001);
  EXPECT_NEAR(2.0 / 3.5,b[1],0.0001);
  EXPECT_NEAR(0.5 / 3.5,b[2],0.0001);

  beliefMDP->setBeliefs(updater2);
  b = updater->getBeliefs();
  EXPECT_NEAR(1.0 / 3.5,b[0],0.0001);
  EXPECT_NEAR(2.0 / 3.5,b[1],0.0001);
  EXPECT_NEAR(0.5 / 3.5,b[2],0.0001);

  // in case this has somehow changed the beliefs of the setter
  b = updater2->getBeliefs();
  EXPECT_NEAR(1.0 / 3.5,b[0],0.0001);
  EXPECT_NEAR(2.0 / 3.5,b[1],0.0001);
  EXPECT_NEAR(0.5 / 3.5,b[2],0.0001);
  
  Observation prevObs;
  Observation currentObs;
  for (unsigned int i = 0; i < 5; i++) {
    trueAgents[i]->setAction(Action::LEFT);
    for (unsigned int j = 0; j < 3; j++)
      modelsDummy[j][i]->setAction(Action::LEFT);
  }
  world->generateObservation(prevObs);
  Action::Type lastAction = Action::LEFT;
  world->step();
  world->generateObservation(currentObs);
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // in case this has somehow changed the beliefs of the setter
  b = updater2->getBeliefs();
  EXPECT_NEAR(1.0 / 3.5,b[0],0.0001);
  EXPECT_NEAR(2.0 / 3.5,b[1],0.0001);
  EXPECT_NEAR(0.5 / 3.5,b[2],0.0001);
}
