/*
File: World.cpp
Author: Samuel Barrett
Description: tests the world controller
Created:  2011-08-29
Modified: 2011-08-29
*/

#include <gtest/gtest.h>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>
#include "AgentDummyTest.h"

class WorldTest: public ::testing::Test {
public:
  WorldTest():
    rng(new RNG(0)),
    model(new WorldModel(Point2D(5,5))),
    world(rng,model,0.0)
  {
    for (int i = 0; i < 5; i++) {
      boost::shared_ptr<AgentDummyTest> agent(new AgentDummyTest(rng,Point2D(5,5)));
      agents.push_back(agent);
      abstractAgents.push_back(agent);
      if (i == 0)
        world.addAgent(AgentModel(i,i,PREY),agents[i]);
      else
        world.addAgent(AgentModel(i,i,PREDATOR),agents[i]);
    }
  }

  double getOutcomeProbApprox(int startPositions[5][2], int endPositions[5][2], ActionProbs actions[5]) {
    std::cout << "------------------------" << std::endl;
    Observation prevObs;
    Observation currentObs;
    for (unsigned int i = 0; i < 5; i++) {
      prevObs.positions.push_back(Point2D(startPositions[i][0],startPositions[i][1]));
      currentObs.positions.push_back(Point2D(endPositions[i][0],endPositions[i][1]));
      agents[i]->setAction(actions[i]);
    }
    return world.getOutcomeProbApprox(prevObs,currentObs,abstractAgents);
  }

protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> model;
  World world;
  std::vector<boost::shared_ptr<AgentDummyTest> > agents;
  std::vector<boost::shared_ptr<Agent> > abstractAgents;
};

TEST_F(WorldTest,NumSteps) {
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ((unsigned int)0,agents[i]->numSteps);
  world.step();
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ((unsigned int)1,agents[i]->numSteps);
  world.step();
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ((unsigned int)2,agents[i]->numSteps);
}

TEST_F(WorldTest,RandomizePositions) {
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ(Point2D(i,i),model->getAgentPosition(i));
  world.randomizePositions();
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_NE(Point2D(i,i),model->getAgentPosition(i)); // not a good test, but holds for this rng
}

TEST_F(WorldTest,Collisions) {
  for (unsigned int i = 0; i < agents.size(); i++)
    model->setAgentPosition(i,Point2D(i,0));
  std::vector<Point2D> requestedPositions;
  for (unsigned int i = 0; i < agents.size(); i++) {
    if (i == 0)
      requestedPositions.push_back(Point2D(0,0));
    else
      requestedPositions.push_back(Point2D(i,0));
  }
  world.handleCollisions(requestedPositions);
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ(Point2D(i,0),model->getAgentPosition(i));

  for (unsigned int i = 0; i < agents.size(); i++)
    requestedPositions[i] = Point2D(i,0);
  // test swaps
  requestedPositions[0] = Point2D(1,0);
  requestedPositions[1] = Point2D(2,0);
  
  world.handleCollisions(requestedPositions);
  for (unsigned int i = 0; i < agents.size(); i++)
    EXPECT_EQ(Point2D(i,0),model->getAgentPosition(i));
}

TEST_F(WorldTest,OutcomeProbApprox) {
  double outcomeProb;
  int startPositions[5][2];
  int endPositions[5][2];
  ActionProbs actions[5];
  for (int i = 0; i < 5; i++) {
    startPositions[i][0] = i;
    startPositions[i][1] = 0;
    endPositions[i][0] = i;
    endPositions[i][1] = 0;
    actions[i] = ActionProbs(Action::NOOP);
  }
  
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_DOUBLE_EQ(1.0,outcomeProb);

  endPositions[0][1] = 1;
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_DOUBLE_EQ(0.0,outcomeProb);

  actions[0] = ActionProbs(Action::RANDOM);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.2,outcomeProb,0.001);

  actions[0] = ActionProbs(Action::UP);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(1.0,outcomeProb,0.001);
  
  actions[0] = ActionProbs(Action::DOWN);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.0,outcomeProb,0.001);
  
  endPositions[0][1] = 0;
  actions[0] = ActionProbs(Action::RIGHT);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(1.0,outcomeProb,0.001);
  
  actions[0] = ActionProbs(Action::LEFT);
  actions[1] = ActionProbs(Action::RIGHT);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(1.0,outcomeProb,0.001);
  
  actions[0] = ActionProbs(Action::RIGHT);
  actions[1] = ActionProbs(Action::UP);
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.0,outcomeProb,0.001);

  endPositions[1][1] = 1;
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.5,outcomeProb,0.001);
  
  endPositions[0][0] = 1;
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.5,outcomeProb,0.001);
  
  endPositions[0][0] = 3;
  outcomeProb = getOutcomeProbApprox(startPositions,endPositions,actions);
  EXPECT_NEAR(0.0,outcomeProb,0.001);
}
