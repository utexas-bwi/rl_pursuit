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
