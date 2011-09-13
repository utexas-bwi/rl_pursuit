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

TEST(WorldMDP,GetSetPositions) {
  Observation obs;
  std::vector<Point2D> positions(5);
  Point2D dims(5,5);
  RNG rng(0);
  unsigned int numTests = 50;
  State_t state;

  getPositionsFromState(0,dims,positions);
  for (unsigned int j = 0; j < 5; j++) {
    EXPECT_EQ(Point2D(0,0),positions[j]);
  }

  for (unsigned int i = 0; i < numTests; i++) {
    obs.positions.clear();
    for (unsigned int j = 0; j < 5; j++) {
      obs.positions.push_back(Point2D(rng.randomInt(dims.x),rng.randomInt(dims.y)));
    }
    state = getStateFromObs(dims,obs);
    getPositionsFromState(state,dims,positions);
    for (unsigned int j = 0; j < 5; j++) {
      ASSERT_EQ(obs.positions[j],positions[j]);
    }
  }
}
