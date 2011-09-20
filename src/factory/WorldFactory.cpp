#include "WorldFactory.h"

boost::shared_ptr<WorldModel> createWorldModel(const Point2D &dims) {
  return boost::shared_ptr<WorldModel>(new WorldModel(dims));
}

boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model) {
  return boost::shared_ptr<World>(new World(rng,model));
}

boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, const Point2D &dims) {
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  return createWorld(rng,model);
}

boost::shared_ptr<World> createWorld(unsigned int randomSeed, boost::shared_ptr<WorldModel> model) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorld(rng,model);
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<World> world, const Json::Value &options) {
  const Json::Value agents = options["agents"];
  boost::shared_ptr<Agent> agent;
  Point2D dims = world->getModel()->getDims();

  unsigned int randomNum = rng->randomUInt(); // used for things like choosing which student team to use
  for (unsigned int i = 0; i < agents.size(); ++i) {
    agent = createAgent(rng->randomUInt(), dims, randomNum, agents[i], options);
    world->addAgent(AgentModel(0,0,getAgentType(agents[i].get("type","NONE").asString())),agent,true);
  }
  return world;
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,model);
  return createWorldAgents(rng,world,options);
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, const Point2D &dims, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,dims);
  return createWorldAgents(rng,world,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, const Point2D &dims, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,dims,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, const Json::Value &options) {
  return createWorldAgents(randomSeed,getDims(options),options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, boost::shared_ptr<WorldModel> model, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,model,options);
}

Point2D getDims(const Json::Value &options) {
  return Point2D(options.get("width",5).asInt(),options.get("height",5).asInt());
}
