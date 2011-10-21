/*
File: WorldMDP.cpp
Author: Samuel Barrett
Description: test the world mdp
Created:  2011-09-09
Modified: 2011-09-09
*/

#include <gtest/gtest.h>
#include <controller/WorldMDP.h>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>
#include <factory/WorldFactory.h>
#include <factory/PlanningFactory.h>
#include "AgentDummyTest.h"

TEST(WorldMDP,GetSetPositions) {
  Observation obs;
  std::vector<Point2D> positions(5);
  Point2D dims(50,50);
  RNG rng(0);
  unsigned int numTests = 5000;
  State_t state;

  getPositionsFromState(0,dims,positions);
  // the first agent gets put in the center of the grid
  EXPECT_EQ(0.5f * dims,positions[0]);
  for (unsigned int j = 1; j < 5; j++) {
    EXPECT_EQ(Point2D(0,0),positions[j]);
  }

  for (unsigned int i = 0; i < numTests; i++) {
    obs.positions.clear();
    for (unsigned int j = 0; j < 5; j++) {
      obs.positions.push_back(Point2D(rng.randomInt(dims.x),rng.randomInt(dims.y)));
    }
    state = getStateFromObs(dims,obs);
    getPositionsFromState(state,dims,positions);
    Point2D diff = 0.5f * dims - obs.positions[0];
    for (unsigned int j = 0; j < 5; j++) {
      ASSERT_EQ(movePosition(dims,obs.positions[j],diff),positions[j]);
    }
  }
}

class WorldMDPTest: public ::testing::Test {
public:
  WorldMDPTest():
    rng(new RNG(0)),
    dims(5,5),
    mdp(createWorldMDP(rng,dims,false,NO_MODEL_UPDATES,StateConverter(5,5),0.0)),
    model(mdp->model),
    adhocInd(1)
  {
    boost::shared_ptr<AgentDummyTest> agent;
    for (int i = 0; i < 5; i++) {
      agent = boost::shared_ptr<AgentDummyTest>(new AgentDummyTest(rng,Point2D(5,5)));
      agents.push_back(agent);
      agentsAbstract.push_back(agent);
    }
    std::vector<AgentModel> agentModels;
    createAgentModels(adhocInd-1,agentModels);
    for (int i = 0; i < 5; i++)
      agentModels[i].pos = Point2D(i,i);
    mdp->addAgents(agentModels,agentsAbstract);
    mdp->adhocAgent = agents[adhocInd];
  }

protected:
  boost::shared_ptr<RNG> rng;
  Point2D dims;
  boost::shared_ptr<WorldMDP> mdp;
  boost::shared_ptr<WorldModel> model;
  std::vector<boost::shared_ptr<AgentDummyTest> > agents;
  std::vector<boost::shared_ptr<Agent> > agentsAbstract;
  unsigned int adhocInd;
};

TEST_F(WorldMDPTest,TakeAction) {
  Observation obs;
  for (int i = 0; i < 5; i++) {
    Point2D pos(i,0);
    model->setAgentPosition(i,pos);
    obs.positions.push_back(pos);
    agents[i]->setAction(Action::NOOP);
  }
  agents[1]->setAction(Action::UP);
  
  State_t startState = getStateFromObs(dims,obs);
  EXPECT_EQ(startState,mdp->getState(obs));
  
  Action::Type action = Action::NOOP;
  float reward;
  State_t state;
  bool terminal;
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(0u,agents[i]->numSteps);
  mdp->takeAction(action,reward,state,terminal);
  EXPECT_EQ(0,reward);
  EXPECT_EQ(startState,state);
  EXPECT_FALSE(terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,agents[i]->numSteps);
  Point2D offset(2,2);
  Point2D start;
  std::vector<Point2D> positions(5);
  getPositionsFromState(state,dims,positions);
  for (int i = 0; i < 5; i++) {
    start = Point2D(i,0);
    EXPECT_EQ(movePosition(dims,start,offset),positions[i]);
  }

  action = Action::UP;
  mdp->takeAction(action,reward,state,terminal);
  EXPECT_EQ(0,reward);
  EXPECT_FALSE(terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(2u,agents[i]->numSteps);
  
  getPositionsFromState(state,dims,positions);
  for (int i = 0; i < 5; i++) {
    if (i == 1)
      start = Point2D(1,1);
    else
      start = Point2D(i,0);
    EXPECT_EQ(movePosition(dims,start,offset),positions[i]);
  }
  
  model->setAgentPosition(0,Point2D(2,2));
  model->setAgentPosition(1,Point2D(0,2));
  model->setAgentPosition(2,Point2D(4,2));
  model->setAgentPosition(3,Point2D(2,0));
  model->setAgentPosition(4,Point2D(2,4));
  agents[0]->setAction(Action::NOOP);
  agents[1]->setAction(Action::NOOP);
  agents[2]->setAction(Action::LEFT);
  agents[3]->setAction(Action::UP);
  agents[4]->setAction(Action::DOWN);
  mdp->takeAction(Action::RIGHT,reward,state,terminal);
  EXPECT_EQ(1.0,reward);
  EXPECT_TRUE(terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(3u,agents[i]->numSteps);
  
  getPositionsFromState(state,dims,positions);
  EXPECT_EQ(Point2D(2,2),positions[0]);
  EXPECT_EQ(Point2D(1,2),positions[1]);
  EXPECT_EQ(Point2D(3,2),positions[2]);
  EXPECT_EQ(Point2D(2,1),positions[3]);
  EXPECT_EQ(Point2D(2,3),positions[4]);
}

TEST_F(WorldMDPTest,SetAgents) {
  std::vector<boost::shared_ptr<AgentDummyTest> > newAgents;
  std::vector<boost::shared_ptr<Agent> > newAbstractAgents;
  for (int i = 0; i < 5; i++) {
    boost::shared_ptr<AgentDummyTest> agent(new AgentDummyTest(rng,Point2D(5,5)));
    newAgents.push_back(agent);
    newAbstractAgents.push_back(agent);
  }
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(0u,agents[i]->numSteps);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(0u,newAgents[i]->numSteps);

  Action::Type action = Action::NOOP;
  float reward;
  State_t state;
  bool terminal;
  mdp->takeAction(action,reward,state,terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,agents[i]->numSteps);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(0u,newAgents[i]->numSteps);
  // set the agents
  mdp->setAgents(newAbstractAgents);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,agents[i]->numSteps);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(0u,newAgents[i]->numSteps);
  mdp->takeAction(action,reward,state,terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,agents[i]->numSteps);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,newAgents[i]->numSteps);
  mdp->takeAction(action,reward,state,terminal);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(1u,agents[i]->numSteps);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(2u,newAgents[i]->numSteps);
}
