#include "WorldFactory.h"
#include <fstream>

int getReplacementInd(unsigned int trialNum) {
  std::ifstream in("data/replacementInds.txt");
  unsigned int ind;
  for (int i = (int)trialNum; i >= 0; i--) {
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

boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, double actionNoise, bool centerPrey) {
  return boost::shared_ptr<World>(new World(rng,model,actionNoise,centerPrey));
}

boost::shared_ptr<World> createWorld(boost::shared_ptr<RNG> rng, const Point2D &dims, double actionNoise, bool centerPrey) {
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  return createWorld(rng,model,actionNoise,centerPrey);
}

boost::shared_ptr<World> createWorld(unsigned int randomSeed, boost::shared_ptr<WorldModel> model, double actionNoise, bool centerPrey) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorld(rng,model,actionNoise,centerPrey);
}

void createAgentModels(int replacementInd, std::vector<AgentModel> &agentModels) {
  int numPredators = 4; // can later change this to an option
  agentModels.push_back(AgentModel(0,0,PREY));
  for (int predatorInd = 0; predatorInd < numPredators; predatorInd++) {
    if (predatorInd == replacementInd)
      agentModels.push_back(AgentModel(0,0,ADHOC));
    else
      agentModels.push_back(AgentModel(0,0,PREDATOR));
  }
}

void createAgentControllersAndModels(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options, std::vector<boost::shared_ptr<Agent> > &agentControllers, std::vector<AgentModel> &agentModels) {
  createAgentControllersAndModels(rng,dims,trialNum,replacementInd,options,boost::shared_ptr<Agent>(),agentControllers,agentModels);
}

void createAgentControllersAndModels(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options, boost::shared_ptr<Agent> adhocAgent, std::vector<boost::shared_ptr<Agent> > &agentControllers, std::vector<AgentModel> &agentModels) {
  std::string prey = options.get("prey","random").asString();
  std::string predator = options.get("predator","greedy").asString();
  std::string adhoc = options.get("adhoc","greedy").asString();
  const Json::Value preyOptions = options["preyOptions"];
  const Json::Value predatorOptions = options["predatorOptions"];
  const Json::Value adhocOptions = options["adhocOptions"];
  bool sharePredator = predatorOptions.get("shared",false).asBool();

  createAgentModels(replacementInd,agentModels);
  
  boost::shared_ptr<Agent> agent;
  boost::shared_ptr<Agent> agentPredator;
  unsigned int predatorInd = 0;
  for (unsigned int agentInd = 0; agentInd < agentModels.size(); agentInd++) {
    if (agentModels[agentInd].type == PREY) {
      agent = createAgent(rng->randomUInt(),dims,prey,trialNum,0,preyOptions,options);
    } else if (agentModels[agentInd].type == PREDATOR) {
      if (sharePredator && (agentPredator.get() != NULL)) {
        agent = agentPredator;
      } else {
        agent = createAgent(rng->randomUInt(),dims,predator,trialNum,predatorInd,predatorOptions,options);
        agentPredator = agent;
      }
      predatorInd++;
    } else if (agentModels[agentInd].type == ADHOC) {
      if (adhocAgent.get() != NULL)
        agent = adhocAgent;
      else
        agent = createAgent(rng->randomUInt(),dims,adhoc,trialNum,predatorInd,adhocOptions,options);
      predatorInd++;
    } else {
      std::cerr << "createAgentControllersAndModels: ERROR, unhandled agent type: " << agentModels[agentInd].type << std::endl;
      exit(56);
    }
    agentControllers.push_back(agent);
  }
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<World> world, unsigned int trialNum, const Json::Value &options) {
  Point2D dims = world->getModel()->getDims();
  
  int replacementInd = getReplacementInd(trialNum);
  
  std::vector<boost::shared_ptr<Agent> > agentControllers;
  std::vector<AgentModel> agentModels;
  createAgentControllersAndModels(rng,dims,trialNum,replacementInd,options,agentControllers,agentModels);
  for (unsigned int i = 0; i < agentControllers.size(); i++) {
    world->addAgent(agentModels[i],agentControllers[i],true);
  }

  return world;
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, unsigned int trialNum, double actionNoise, bool centerPrey, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,model,actionNoise,centerPrey);
  return createWorldAgents(rng,world,trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(boost::shared_ptr<RNG> rng, const Point2D &dims, unsigned int trialNum, double actionNoise, bool centerPrey, const Json::Value &options) {
  boost::shared_ptr<World> world = createWorld(rng,dims,actionNoise,centerPrey);
  return createWorldAgents(rng,world,trialNum,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, double actionNoise, bool centerPrey, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,dims,trialNum,actionNoise,centerPrey,options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, unsigned int trialNum, const Json::Value &options) {
  return createWorldAgents(randomSeed,getDims(options),trialNum,getActionNoise(options),getCenterPrey(options),options);
}

boost::shared_ptr<World> createWorldAgents(unsigned int randomSeed, boost::shared_ptr<WorldModel> model, unsigned int trialNum, double actionNoise, bool centerPrey, const Json::Value &options) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createWorldAgents(rng,model,trialNum,actionNoise,centerPrey, options);
}

Point2D getDims(const Json::Value &options) {
  return Point2D(options.get("width",5).asInt(),options.get("height",5).asInt());
}

double getActionNoise(const Json::Value &options) {
  return options.get("actionNoise",0).asDouble();
}

bool getCenterPrey(const Json::Value &options) {
  return options.get("centerPrey",true).asBool();
}
