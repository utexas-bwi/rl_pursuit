#include <gtest/gtest.h>
#include <set>
#include <common/RNG.h>
#include <planning/UCTEstimator.h>

class TestUCT : public ::testing::Test {
public:
  TestUCT():
    numActions(3),
    lambda(0),
    gamma(0),
    rewardBound(1.0),
    rewardRangePerStep(-1.0),
    initialValue(0.0),
    initialStateVisits(0),
    initialStateActionVisits(),
    unseenValue(999999),
    rng(new RNG(0))
  {
  }
protected:
  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

  virtual void createUCT() {
    uct = boost::shared_ptr<UCTEstimator<int,unsigned int> >(new UCTEstimator<int,unsigned int>(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue));
  }

  virtual void runLambdaGammaTest(float lambda,float gamma,unsigned int numActions, int states[], unsigned int actions[], float rewards[]) {
    this->lambda = lambda;
    this->gamma = gamma;
    createUCT();
    uct->startRollout(states[0]);
    for (unsigned int i = 0; i < numActions; i++)
      uct->visit(actions[i],rewards[i],states[i+1]);
    uct->finishRollout(true);

    float val = rewards[numActions-1];
    for (int i = numActions-1; i >= 0; i--) {
      for (unsigned int a = 0; a < 3; a++) {
        //std::cerr << i << " " << a << " " << val << " " << uct->getStateActionValue(states[i],a) << std::endl;
        if (a == actions[i])
          EXPECT_EQ(val,uct->getStateActionValue(states[i],a));
        else
          EXPECT_EQ(0.0,uct->getStateActionValue(states[i],a));
      }
      val *= gamma * lambda;
    }
  }
  
  unsigned int numActions;
  float lambda;
  float gamma;
  float rewardBound;
  float rewardRangePerStep;
  float initialValue;
  unsigned int initialStateVisits;
  unsigned int initialStateActionVisits;
  float unseenValue;
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<UCTEstimator<int,unsigned int> > uct;
};

TEST_F(TestUCT,UnseenPlanningState) {
  int state = 0;
  createUCT();
  std::set<unsigned int> actions;
  for (int i = 0; i < 20; i++)
    actions.insert(uct->selectPlanningAction(state));
  EXPECT_EQ(numActions,actions.size());
}

TEST_F(TestUCT,UnseenWorldState) {
  int state = 0;
  createUCT();
  std::set<unsigned int> actions;
  for (int i = 0; i < 20; i++)
    actions.insert(uct->selectWorldAction(state));

  EXPECT_EQ(numActions,actions.size());
  for (unsigned int action = 0; action < numActions; action++)
    EXPECT_EQ(initialValue,uct->getStateActionValue(state,action));
  EXPECT_EQ(initialValue,uct->maxValueForState(state));
  
  initialValue = 10;
  createUCT();
  for (unsigned int action = 0; action < numActions; action++)
    EXPECT_EQ(initialValue,uct->getStateActionValue(state,action));
  EXPECT_EQ(initialValue,uct->maxValueForState(state));
  
  initialValue = -10;
  createUCT();
  for (unsigned int action = 0; action < numActions; action++)
    EXPECT_EQ(initialValue,uct->getStateActionValue(state,action));
  EXPECT_EQ(initialValue,uct->maxValueForState(state));
}

TEST_F(TestUCT,SimpleLambda0Gamma0) {
  lambda = 0;
  gamma = 0;
  createUCT();
  uct->startRollout(0);
  uct->visit(0,0.0,1);
  uct->visit(0,1.0,2);
  uct->finishRollout(true);

  EXPECT_EQ(0.0,uct->getStateActionValue(0,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,2));
  EXPECT_EQ(1.0,uct->getStateActionValue(1,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,2));
}

TEST_F(TestUCT,SimpleLambda1Gamma0) {
  lambda = 1;
  gamma = 0;
  createUCT();
  uct->startRollout(0);
  uct->visit(0,0.0,1);
  uct->visit(0,1.0,2);
  uct->finishRollout(true);

  EXPECT_EQ(0.0,uct->getStateActionValue(0,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,2));
  EXPECT_EQ(1.0,uct->getStateActionValue(1,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,2));
}

TEST_F(TestUCT,SimpleLambda1Gamma1) {
  lambda = 1;
  gamma = 1;
  createUCT();
  uct->startRollout(0);
  uct->visit(0,0.0,1);
  uct->visit(0,1.0,2);
  uct->finishRollout(true);

  EXPECT_EQ(1.0,uct->getStateActionValue(0,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(0,2));
  EXPECT_EQ(1.0,uct->getStateActionValue(1,0));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,1));
  EXPECT_EQ(0.0,uct->getStateActionValue(1,2));
}

TEST_F(TestUCT,SimpleLambda075Gamma1) {
  int numActions = 3;
  int states[4] = {0,1,2,3};
  unsigned int actions[3] = {0,0,0};
  float rewards[3] = {0,0,1.0};
  runLambdaGammaTest(0.75,1.0,numActions,states,actions,rewards);
}

TEST_F(TestUCT,SimpleLambda1Gamma066) {
  int numActions = 3;
  int states[4] = {0,1,2,3};
  unsigned int actions[3] = {2,0,1};
  float rewards[3] = {0,0,1.0};
  runLambdaGammaTest(1.0,0.66,numActions,states,actions,rewards);
}

TEST_F(TestUCT,SimpleLambda075Gamma066) {
  int numActions = 3;
  int states[4] = {0,1,2,3};
  unsigned int actions[3] = {0,1,2};
  float rewards[3] = {0,0,1.0};
  runLambdaGammaTest(0.75,0.66,numActions,states,actions,rewards);
}
