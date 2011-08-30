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
#include <controller/AgentDummy.h>

class AgentDummyTest: public AgentDummy {
public:
  AgentDummyTest(boost::shared_ptr<RNG> rng, const Point2D &dims):
    AgentDummy(rng,dims),
    numSteps(0)
  {}

  Action::Type step(const Observation &obs) {
    numSteps++;
    return AgentDummy::step(obs);
  }
  unsigned int numSteps;
};

class WorldTest: public ::testing::Test {
public:
  WorldTest():
    rng(new RNG(0)),
    model(new WorldModel(Point2D(5,5))),
    world(rng,model)
  {
    for (int i = 0; i < 5; i++) {
      agents.push_back(boost::shared_ptr<AgentDummyTest>(new AgentDummyTest(rng,Point2D(5,5))));
      if (i == 0)
        world.addAgent(AgentModel(i,i,PREY),agents[i]);
      else
        world.addAgent(AgentModel(i,i,PREDATOR),agents[i]);
    }
  }
protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> model;
  World world;
  std::vector<boost::shared_ptr<AgentDummyTest> > agents;
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
