/*
File: DefaultMap.cpp
Author: Samuel Barrett
Description: tests the DefaultMap class
Created:  2011-08-29
Modified: 2011-08-29
*/

#include <gtest/gtest.h>
#include <common/DefaultMap.h>

TEST(TestDefaultMap,DefaultValue) {
  DefaultMap<int,float> map(1.0);

  EXPECT_FLOAT_EQ(1.0,map.get(1));
  EXPECT_FLOAT_EQ(1.0,map.get(45));
  map[5] += 1.3;
  EXPECT_FLOAT_EQ(2.3,map.get(5));
}

TEST(TestDefaultMap,SetClear) {
  DefaultMap<int,float> map(1.0);
  map.set(5,2.5);
  EXPECT_FLOAT_EQ(2.5,map.get(5));
  EXPECT_FLOAT_EQ(1.0,map.get(6));
  map.clear();
  EXPECT_FLOAT_EQ(1.0,map.get(5));
}

TEST(TestDefaultMap,Sizes) {
  DefaultMap<int,float> map(1.0);
  EXPECT_EQ((unsigned int)0,map.size());
  map.get(5);
  EXPECT_EQ((unsigned int)0,map.size());
  map.set(5,2.5);
  EXPECT_EQ((unsigned int)1,map.size());
  map.get(5);
  EXPECT_EQ((unsigned int)1,map.size());
  map.set(5,3.5);
  EXPECT_EQ((unsigned int)1,map.size());
  map[6] = 1.5;
  EXPECT_EQ((unsigned int)2,map.size());
  map[5] = 6.7;
  EXPECT_EQ((unsigned int)2,map.size());
  map.clear();
  EXPECT_EQ((unsigned int)0,map.size());
}
