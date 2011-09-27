#include "WorldFactory.h"
#include <fstream>

unsigned int getReplacementInd(unsigned int trialNum) {
  std::ifstream in("data/replacementInds.txt");
  unsigned int ind;
  for (; trialNum > 0; trialNum++) {
    in >> ind;
    if (!in.good()) {
      std::cerr << "WorldFactory::getReplacementInd: ERROR file ended before reaching correct trial num" << std::endl;
      exit(14);
    }
  }
  in.close();
  return ind;
}

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

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<World> world, unsigned int trialNum, const Json::Value &options) {
  std::string prey = options.get("prey","random").asString();
  std::string predator = options.get("predator","greedy").asString();
  std::string adhoc = options.get("adhoc","").asString();

  boost::shared_ptr<Agent> agent;
  Point2D dims = world->getModel()->getDims();
  
  unsigned int replacementInd = getReplacementInd(trialNum);

  unsigned int predatorInd = 0;
  for (unsigned int i = 0; i < agents.size(); ++i) {
    agent = createAgent(rng->randomUInt(), dims, trialNum, predatorInd, agents[i], options);
    AgentModel agentModel(0,0,getAgentType(agents[i].get("type","NONE").asString()));
    world->addAgent(agentModel,agent,true);
    
    if ((agentModel.type == PREDATOR) || (agentModel.type == ADHOC))
      predatorInd++;
  }
  return world;
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, unsigned int trialNum, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,model);
  return createWorldAgents(rng,world,trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,dims);
  return createWorldAgents(rng,world,trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,dims,trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, unsigned int trialNum, const Json::Value &options) {
  return createWorldAgents(randomSeed,getDims(options),trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, boost::shared_ptr<WorldModel> model, unsigned int trialNum, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,model,trialNum,options);
}

Point2D getDims(const Json::Value &options) {
  return Point2D(options.get("width",5).asInt(),options.get("height",5).asInt());
}
