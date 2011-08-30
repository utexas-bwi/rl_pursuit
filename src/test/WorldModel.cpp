#include <gtest/gtest.h>
#include <model/WorldModel.h>

class WorldModelTest: public ::testing::Test {
public:
  WorldModelTest():
    model(Point2D(5,5))
  {
    for (int i = 0; i < 5; i++) {
      if (i == 0)
        model.addAgent(AgentModel(i,i,PREY));
      else
        model.addAgent(AgentModel(i,i,PREDATOR));
    }
  }

protected:
  WorldModel model;
};

TEST(WorldModel,Basic) {
  WorldModel model(Point2D(5,5));
  EXPECT_EQ(Point2D(5,5),model.getDims());
  EXPECT_EQ((unsigned int)0,model.getNumAgents());
  EXPECT_TRUE(model.isPreyCaptured());
  model.addAgent(AgentModel(0,0,PREY));
  EXPECT_EQ((unsigned int)1,model.getNumAgents());
  EXPECT_FALSE(model.isPreyCaptured());
  EXPECT_EQ(Point2D(0,0),model.getAgentPosition(0));
  model.addAgent(AgentModel(0,1,PREDATOR));
  EXPECT_EQ((unsigned int)2,model.getNumAgents());
  EXPECT_FALSE(model.isPreyCaptured());
  EXPECT_EQ(Point2D(0,0),model.getAgentPosition(0));
  EXPECT_EQ(Point2D(0,1),model.getAgentPosition(1));
}

TEST_F(WorldModelTest,Set) {
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(Point2D(i,i),model.getAgentPosition(i));
  model.setAgentPosition(1,Point2D(2,1));
  EXPECT_EQ(Point2D(2,1),model.getAgentPosition(1));
  EXPECT_EQ((unsigned int)5,model.getNumAgents());
}

TEST_F(WorldModelTest,Observation) {
  Observation obs;
  model.generateObservation(obs);
  Observation saved_obs(obs);
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(Point2D(i,i),obs.positions[i]);
  EXPECT_EQ(0,obs.preyInd);
  EXPECT_EQ(Point2D(0,0),obs.preyPos());
  obs.myInd = 2;
  EXPECT_EQ(Point2D(2,2),obs.myPos());
  model.setAgentPosition(2,Point2D(3,1));
  model.generateObservation(obs);
  obs.myInd = 2;
  EXPECT_EQ(Point2D(3,1),obs.myPos());

  model.setPositionsFromObservation(saved_obs);
  EXPECT_EQ(Point2D(2,2),model.getAgentPosition(2));
}

TEST_F(WorldModelTest,AddAgent) {
  EXPECT_FALSE(model.addAgent(AgentModel(2,2,PREY)));
  EXPECT_FALSE(model.addAgent(AgentModel(3,3,PREDATOR)));
  EXPECT_TRUE(model.addAgent(AgentModel(3,2,PREDATOR)));
  EXPECT_TRUE(model.addAgent(AgentModel(3,3,PREDATOR),true));
  EXPECT_FALSE(model.addAgent(AgentModel(3,3,PREDATOR)));
  EXPECT_FALSE(model.addAgent(AgentModel(4,6,PREDATOR)));
}

TEST_F(WorldModelTest,Collisions) {
  for (int i = 0; i < 5; i++)
    EXPECT_EQ(i,model.getCollision(Point2D(i,i)));
  EXPECT_LT(model.getCollision(Point2D(2,1)),0);
  EXPECT_LT(model.getCollision(Point2D(3,2)),0);
  EXPECT_LT(model.getCollision(Point2D(0,1)),0);
  for (int i = 0; i < 5; i++) {
    EXPECT_LT(model.getCollision(Point2D(i,i),i),0);
    EXPECT_EQ(i,model.getCollision(Point2D(i,i),i+1));
    EXPECT_EQ(i,model.getCollision(Point2D(i,i),i-1));
  }
  EXPECT_LT(model.getCollision(Point2D(2,2),-1,2),0);
  EXPECT_EQ(2,model.getCollision(Point2D(2,2),-1,3));
}
