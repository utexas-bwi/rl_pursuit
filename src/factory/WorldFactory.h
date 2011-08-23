#ifndef WORLDFACTORY_NNNSUN1M
#define WORLDFACTORY_NNNSUN1M

/*
File: WorldFactory.h
Author: Samuel Barrett
Description: generates a world model controller
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <model/WorldModel.h>
#include <controller/World.h>

void createWorld(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<WorldModel> &worldModel, boost::shared_ptr<World> &worldController, const Json::Value &options = Json::Value());

#endif /* end of include guard: WORLDFACTORY_NNNSUN1M */
