#include <gtest/gtest.h>
#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <model/WorldModel.h>

class WorldTest: public ::testing::Test {
public:
  WorldTest():
    rng(new RNG(0))
  {
  }
protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldModel> model;
};
