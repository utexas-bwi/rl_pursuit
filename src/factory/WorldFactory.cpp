#include "WorldFactory.h"

void createWorld(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<WorldModel> &worldModel, boost::shared_ptr<World> &worldController, const Json::Value &options) {
  worldModel = boost::shared_ptr<WorldModel>(new WorldModel(dims));
  worldController = boost::shared_ptr<World>(new World(rng,worldModel));
}
