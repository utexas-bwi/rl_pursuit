#include "PlanningFactory.h"
#include "WorldFactory.h"
#include <controller/WorldSilverMDP.h>
#include <boost/algorithm/string.hpp>

boost::shared_ptr<WorldMultiModelMDP> createWorldMultiModelMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, const Json::Value &options) {
  // create the world model and controller
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  boost::shared_ptr<World> controller = createWorld(rng->randomUInt(),model);
  // create the dummy agent for the ad hoc agent
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(boost::shared_ptr<RNG>(new RNG(rng->randomUInt())),dims));
  
  // create the agents
  const Json::Value models = options["models"];
  std::vector<std::vector<boost::shared_ptr<Agent> > > modelList(models.size());
  std::vector<double> modelProbs;
  AgentType agentType = PREY;
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProbs.push_back(models[i].get("prob",1.0).asDouble());
    const Json::Value model = models[i]["model"];
    for (unsigned int j = 0; j < model.size(); j++) {
      agentType = getAgentType(model[j].get("type","UNKNOWN").asString());
      if (agentType == ADHOC) {
        modelList[i].push_back(adhocAgent);
      } else {
        modelList[i].push_back(createAgent(rng->randomUInt(),dims,model[j])); // TODO RNG?
      }
      
      // add the first set of agents to the world
      if (i == 0)
        controller->addAgent(AgentModel(0,0,agentType),modelList[i][j],true);
    }
  }

  std::string updateTypeString = options.get("update","bayesian").asString();
  boost::to_lower(updateTypeString);
  ModelUpdateType updateType;

  if (updateTypeString == "bayesian")
    updateType = BAYESIAN_UPDATES;
  else if (updateTypeString == "polynomial")
    updateType = POLYNOMIAL_WEIGHTS;
  else {
    std::cerr << "createWorldMultiModelMDP: ERROR: unknown updateTypeString: " << updateTypeString;
    assert(false);
  }

  if (options.get("silver",false).asBool()) {
    return boost::shared_ptr<WorldMultiModelMDP>(new WorldSilverMDP(rng,model,controller,adhocAgent,modelList,modelProbs,updateType));
  } else {
    return boost::shared_ptr<WorldMultiModelMDP>(new WorldMultiModelMDP(rng,model,controller,adhocAgent,modelList,modelProbs,updateType));
  }
}

///////////////////////////////////////////////////////////////

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, float lambda, float gamma, float rewardBound, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initialStateActionVisits, float unseenValue) {
  return boost::shared_ptr<UCTEstimator<State_t,Action::Type> >(new UCTEstimator<State_t,Action::Type>(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue));
}

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, const Json::Value &options) {
  float lambda = options.get("lambda",0.8).asDouble();
  float gamma = options.get("gamma",0.95).asDouble();
  float unseenValue = options.get("unseenValue",9999999).asDouble();
  float initialValue = options.get("initialValue",0).asDouble();
  float rewardBound = options.get("rewardBound",-1).asDouble();
  float rewardRangePerStep = options.get("rewardRangePerStep",-1).asDouble();

  unsigned int initialStateVisits = options.get("initialStateVisits",0).asUInt();
  unsigned int initialStateActionVisits = options.get("initialStateActionVisits",0).asUInt();
  return createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue);
}

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(unsigned int randomSeed, Action::Type numActions, const Json::Value &options) {
  return createUCTEstimator(boost::shared_ptr<RNG>(new RNG(randomSeed)),numActions,options);
}

///////////////////////////////////////////////////////////////

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth) {
  return boost::shared_ptr<MCTS<State_t,Action::Type> >(new MCTS<State_t,Action::Type>(model,valueEstimator,numPlayouts,maxPlanningTime,maxDepth));
}

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,const Json::Value &options) {
  unsigned int numPlayouts = options.get("playouts",0).asUInt();
  double maxPlanningTime = options.get("time",0.0).asDouble();
  unsigned int maxDepth = options.get("depth",0).asUInt();

  return createMCTS(model,valueEstimator,numPlayouts,maxPlanningTime,maxDepth);
}
