/*
File: ModelUpdaterBayes.cpp
Author: Samuel Barrett
Description: tests ModelUpdaterBayes
Created:  2011-10-18
Modified: 2011-10-18
*/

#include <gtest/gtest.h>
#include <controller/ModelUpdaterBayes.h>
#include "AgentDummyTest.h"

class ModelUpdaterBayesTest: public ::testing::Test {
public:
  ModelUpdaterBayesTest():
    rng(new RNG(0)),
    mdpRng(new RNG(1)),
    worldRng(new RNG(2)),
    dims(5,5),
    model(new WorldModel(dims)),
    world(new World(worldRng,model,0.0)),
    models(3,std::vector<boost::shared_ptr<Agent> >(5,boost::shared_ptr<Agent>())),
    modelsDummy(3,std::vector<boost::shared_ptr<AgentDummyTest> >(5,boost::shared_ptr<AgentDummyTest>())),
    adhocInd(1)
  {
    for (int i = 0; i < 5; i++) {
      boost::shared_ptr<AgentDummyTest> agent(new AgentDummyTest(rng,Point2D(5,5)));
      trueAgents.push_back(agent);
      
      if (i == 0)
        world->addAgent(AgentModel(i,i,PREY),trueAgents[i]);
      else
        world->addAgent(AgentModel(i,i,PREDATOR),trueAgents[i]);
    }
    mdp = boost::shared_ptr<WorldMDP>(new WorldMDP(mdpRng,model,world,trueAgents[adhocInd]));
   
    // create the models
    boost::shared_ptr<AgentDummyTest> agent;
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

    updater = boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(rng,mdp,models,modelPrior,modelDescriptions,BAYESIAN_UPDATES));
  }

  void resetUpdater(ModelUpdateType modelUpdateType) {
    updater = boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(rng,mdp,models,modelPrior,modelDescriptions,modelUpdateType));
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
  boost::shared_ptr<RNG> mdpRng;
  boost::shared_ptr<RNG> worldRng;
  Point2D dims;
  boost::shared_ptr<WorldModel> model;
  boost::shared_ptr<World> world;
  boost::shared_ptr<WorldMDP> mdp;
  std::vector<boost::shared_ptr<AgentDummyTest> > trueAgents;
  boost::shared_ptr<ModelUpdaterBayes> updater;
  std::vector<std::vector<boost::shared_ptr<Agent> > > models;
  std::vector<std::vector<boost::shared_ptr<AgentDummyTest> > > modelsDummy;

  std::vector<double> modelPrior;
  std::vector<std::string> modelDescriptions;
  unsigned int adhocInd;
};

TEST_F(ModelUpdaterBayesTest,UpdateControllerInformation) {
  Observation prevObs;

  checkNumSteps(trueAgents,0);
  checkModelsSteps(0);
  
  model->generateObservation(prevObs);
  world->step();
  checkNumSteps(trueAgents,1);
  checkModelsSteps(0);
  updater->updateControllerInformation(prevObs);
  checkNumSteps(trueAgents,1);
  checkModelsSteps(1);
  
  model->generateObservation(prevObs);
  world->step();
  checkNumSteps(trueAgents,2);
  checkModelsSteps(1);
  updater->updateControllerInformation(prevObs);
  checkNumSteps(trueAgents,2);
  checkModelsSteps(2);
}

TEST_F(ModelUpdaterBayesTest,BayesianActionUpdates) {
  std::vector<double> probs;
  for (unsigned int i = 0; i < 5; i++) {
    trueAgents[i]->setAction(Action::LEFT);
    for (unsigned int j = 0; j < 3; j++)
      modelsDummy[j][i]->setAction(Action::LEFT);
  }
  modelsDummy[1][3]->setAction(Action::RIGHT);
  ActionProbs a;
  a[Action::LEFT] = 0.75;
  a[Action::RIGHT] = 0.25;
  modelsDummy[2][3]->setAction(a);
  
  // check probs before starting
  probs = updater->getBeliefs();
  for (unsigned int i = 0; i < probs.size(); i++)
    EXPECT_NEAR(0.33333333,probs[i],0.00001);

  Observation prevObs;
  Observation currentObs;
  Action::Type lastAction = Action::LEFT;
  model->generateObservation(prevObs);
  world->step();
  // change the ad hoc agent's action, to make sure the lastAction is being used
  trueAgents[adhocInd]->setAction(Action::UP);

  model->generateObservation(currentObs);
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // check probs after 1 step
  probs = updater->getBeliefs();
  double probCorrect = 1.0 / 1.75;
  double probClose = 0.75 / 1.75;
  EXPECT_NEAR(probCorrect,probs[0],0.00001); // correct should be growing
  EXPECT_EQ(0,probs[1]);
  EXPECT_NEAR(probClose,probs[2],0.00001); // close should be falling off
  
  trueAgents[adhocInd]->setAction(Action::DOWN);
  model->generateObservation(prevObs);
  world->step();
  // change the ad hoc agent's action, to make sure the lastAction is being used
  trueAgents[adhocInd]->setAction(Action::UP);
  lastAction = Action::DOWN;

  model->generateObservation(currentObs);
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // check probs after 1 step
  probCorrect *= 1.0 / 1.75;
  probClose *= 0.75 / 1.75;
  double temp = probCorrect;
  probCorrect /= (temp + probClose);
  probClose /= (temp + probClose);
  probs = updater->getBeliefs();
  EXPECT_NEAR(probCorrect,probs[0],0.00001); // correct should be growing
  EXPECT_EQ(0,probs[1]);
  EXPECT_NEAR(probClose,probs[2],0.00001); // close should be falling off

  //check if all model probabilites are too low, there should be no update
  trueAgents[adhocInd]->setAction(Action::LEFT);
  model->generateObservation(prevObs);
  world->step();
  model->generateObservation(currentObs);
  lastAction = Action::DOWN;
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // should be no change
  probs = updater->getBeliefs();
  EXPECT_NEAR(probCorrect,probs[0],0.00001); // correct should be growing
  EXPECT_EQ(0,probs[1]);
  EXPECT_NEAR(probClose,probs[2],0.00001); // close should be falling off

}

TEST_F(ModelUpdaterBayesTest,PolynomialActionUpdates) {
  resetUpdater(POLYNOMIAL_WEIGHTS);
  double eta = 0.5;

  std::vector<double> probs;
  std::vector<double> expectedProbs(3,0);
  for (unsigned int i = 0; i < 5; i++) {
    trueAgents[i]->setAction(Action::LEFT);
    for (unsigned int j = 0; j < 3; j++)
      modelsDummy[j][i]->setAction(Action::LEFT);
  }
  modelsDummy[1][3]->setAction(Action::RIGHT);
  ActionProbs a;
  a[Action::LEFT] = 0.75;
  a[Action::RIGHT] = 0.25;
  modelsDummy[2][3]->setAction(a);
  
  // check probs before starting
  for (int i = 0; i < 3; i++)
    expectedProbs[i] = 1.0;
  updater->normalizeModelProbs(expectedProbs);
  
  probs = updater->getBeliefs();
  for (unsigned int i = 0; i < probs.size(); i++)
    EXPECT_NEAR(expectedProbs[i],probs[i],0.00001);

  Observation prevObs;
  Observation currentObs;
  Action::Type lastAction = Action::LEFT;
  model->generateObservation(prevObs);
  world->step();
  // change the ad hoc agent's action, to make sure the lastAction is being used
  trueAgents[adhocInd]->setAction(Action::UP);

  model->generateObservation(currentObs);
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // check probs after 1 step
  probs = updater->getBeliefs();

  expectedProbs[0] *= 1.0;
  expectedProbs[1] *= (1 - eta);
  expectedProbs[2] *= (1 - eta * 0.25);
  updater->normalizeModelProbs(expectedProbs);
  for (unsigned int i = 0; i < expectedProbs.size(); i++)
    EXPECT_NEAR(expectedProbs[i],probs[i],0.00001);
  
  trueAgents[adhocInd]->setAction(Action::DOWN);
  model->generateObservation(prevObs);
  world->step();
  // change the ad hoc agent's action, to make sure the lastAction is being used
  trueAgents[adhocInd]->setAction(Action::UP);
  lastAction = Action::DOWN;

  model->generateObservation(currentObs);
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // check probs after 1 step
  expectedProbs[0] *= 1.0;
  expectedProbs[1] *= (1 - eta);
  expectedProbs[2] *= (1 - eta * 0.25);
  updater->normalizeModelProbs(expectedProbs);
  probs = updater->getBeliefs();
  for (unsigned int i = 0; i < expectedProbs.size(); i++)
    EXPECT_NEAR(expectedProbs[i],probs[i],0.00001);

  //check if all model probabilites are too low, there should be no update
  trueAgents[adhocInd]->setAction(Action::LEFT);
  model->generateObservation(prevObs);
  world->step();
  model->generateObservation(currentObs);
  lastAction = Action::DOWN;
  updater->updateRealWorldAction(prevObs,lastAction,currentObs);
  // should be no change
  probs = updater->getBeliefs();
  for (unsigned int i = 0; i < expectedProbs.size(); i++)
    EXPECT_NEAR(expectedProbs[i],probs[i],0.00001);
}

TEST_F(ModelUpdaterBayesTest,AdvancedTests) {
  // test the sampling
  modelPrior[0] = 1.0;
  modelPrior[1] = 0.5;
  modelPrior[2] = 1.5;
  resetUpdater(BAYESIAN_UPDATES);
  updater->normalizeModelProbs(modelPrior);
  unsigned int numSamples = 100000;

  std::vector<unsigned int> sampleCounts(3,0);
  for (unsigned int i = 0; i < numSamples; i++)
    sampleCounts[updater->selectModelInd(0)]++;
  for (unsigned int i = 0; i < 3; i++)
    EXPECT_NEAR(modelPrior[i], sampleCounts[i] / (double)numSamples, 0.01);
}

TEST_F(ModelUpdaterBayesTest,CopyModel) {
  std::vector<boost::shared_ptr<Agent> > copy;

  updater->copyModel(1,copy);
  std::vector<boost::shared_ptr<AgentDummyTest> > copyDummy;
  for (unsigned int i = 0; i < copy.size(); i++)
    copyDummy.push_back(boost::static_pointer_cast<AgentDummyTest>(copy[i]));

  checkNumSteps(modelsDummy[1],0);
  checkNumSteps(copyDummy,0);
  EXPECT_EQ(0u,modelsDummy[1][adhocInd]->numSteps);
  EXPECT_EQ(0u,copyDummy[adhocInd]->numSteps);

  world->step(models[1]);
  checkNumSteps(modelsDummy[1],1);
  checkNumSteps(copyDummy,0);
  EXPECT_EQ(1u,modelsDummy[1][adhocInd]->numSteps);
  EXPECT_EQ(1u,copyDummy[adhocInd]->numSteps);
  
  world->step(copy);
  checkNumSteps(modelsDummy[1],1);
  checkNumSteps(copyDummy,1);
  EXPECT_EQ(2u,modelsDummy[1][adhocInd]->numSteps);
  EXPECT_EQ(2u,copyDummy[adhocInd]->numSteps);
  
  world->step(copy);
  checkNumSteps(modelsDummy[1],1);
  checkNumSteps(copyDummy,2);
  EXPECT_EQ(3u,modelsDummy[1][adhocInd]->numSteps);
  EXPECT_EQ(3u,copyDummy[adhocInd]->numSteps);
}
