/*
File: PlanningFactory.cpp
Author: Samuel Barrett
Description: generates objects for planning
Created:  2011-08-24
Modified: 2011-10-02
*/

#include "PlanningFactory.h"
#include <boost/algorithm/string.hpp>
//#include <controller/WorldSilverMDP.h>
//#include <controller/WorldSilverWeightedMDP.h>
#include <controller/ModelUpdaterBayes.h>
#include <controller/ModelUpdaterSilver.h>
#include <planning/DualUCTEstimator.h>
#include "WorldFactory.h"
#include <controller/State.h>
#include <controller/WorldBeliefMDP.h>

ModelUpdateType getModelUpdateType(std::string type) {
  boost::to_lower(type);
  if (type == "bayesian")
    return BAYESIAN_UPDATES;
  else if (type == "polynomial")
    return POLYNOMIAL_WEIGHTS;
  else if (type == "none")
    return NO_MODEL_UPDATES;
  else {
    std::cerr << "getModelUpdateType: ERROR: unknown updateTypeString: " << type;
    exit(34);
  }
}

boost::shared_ptr<RNG> makeRNG(unsigned int seed) {
  return boost::shared_ptr<RNG>(new RNG(seed));
}

// MODEL UPDATER
boost::shared_ptr<ModelUpdaterBayes> createModelUpdaterBayes(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<std::vector<boost::shared_ptr<Agent> > > &modelList, const std::vector<double> &modelProbs, const std::vector<std::string> &modelDescriptions, ModelUpdateType updateType) {
    return boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(rng,mdp,modelList,modelProbs,modelDescriptions,updateType));
}

boost::shared_ptr<ModelUpdater> createModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, boost::shared_ptr<Agent> adhocAgent, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options) {
  // create the agents
  const Json::Value models = options["models"];
  std::vector<std::vector<boost::shared_ptr<Agent> > > modelList(models.size());
  std::vector<double> modelProbs;
  std::vector<std::string> modelDescriptions;
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProbs.push_back(models[i].get("prob",1.0).asDouble());
    modelDescriptions.push_back(models[i].get("desc","NO DESCRIPTION").asString());

    std::vector<AgentModel> agentModels;
    createAgentControllersAndModels(rng,dims,trialNum,replacementInd,models[i],adhocAgent,modelList[i],agentModels);
    
    // add the first set of agents to the world
    if (i == 0)
      mdp->addAgents(agentModels,modelList[i]);
  }

  if (options.get("silver",false).asBool()) {
    // make a silver updater
    bool weighted = options.get("weighted",false).asBool();
    return boost::shared_ptr<ModelUpdaterSilver>(new ModelUpdaterSilver(rng,mdp,modelList,modelProbs,modelDescriptions,weighted));
  } else {
    // make a bayes updater
    std::string updateTypeString = options.get("update","bayesian").asString();
    ModelUpdateType updateType = getModelUpdateType(updateTypeString);

    return createModelUpdaterBayes(rng,mdp,modelList,modelProbs,modelDescriptions,updateType);
  }
}

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, bool beliefMDP, ModelUpdateType updateType, const StateConverter &stateConverter, double actionNoise) {
  // create some rngs
  boost::shared_ptr<RNG> rngWorld1 = makeRNG(rng->randomUInt());
  boost::shared_ptr<RNG> rngWorld2 = makeRNG(rng->randomUInt());
  boost::shared_ptr<RNG> rngMDP1 = makeRNG(rng->randomUInt());
  boost::shared_ptr<RNG> rngMDP2 = makeRNG(rng->randomUInt());
  boost::shared_ptr<RNG> rngModelUpdater = makeRNG(rng->randomUInt());
  boost::shared_ptr<RNG> rngDummy = makeRNG(rng->randomUInt());
  //boost::shared_ptr<RNG> rngOther = makeRNG(rng->randomUInt());

  if (!beliefMDP) {
    rngWorld1 = rngWorld2;
    rngMDP1 = rngMDP2;
  }

  // create the world model and controller
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  boost::shared_ptr<World> controller = createWorld(rngWorld1,model,actionNoise);
  // create the dummy agent for the ad hoc agent
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(rngDummy,dims));

  boost::shared_ptr<WorldMDP> mdp(new WorldMDP(rngMDP1,model,controller,adhocAgent));
  if (! beliefMDP)
    return mdp;

  //model = createWorldModel(dims);
  controller = createWorld(rngWorld2,model,actionNoise);

  //std::cout << "HERE" << std::endl;
  //controller = createWorldAgents(rngOther,controller,0,Json::Value());
  //std::cout << "HERE2" << std::endl;
 
  boost::shared_ptr<ModelUpdaterBayes> modelUpdater = createModelUpdaterBayes(rngModelUpdater,mdp,std::vector<std::vector<boost::shared_ptr<Agent> > >(),std::vector<double>(),std::vector<std::string>(),updateType);
  return boost::shared_ptr<WorldMDP>(new WorldBeliefMDP(rngMDP2,model,controller,adhocAgent,stateConverter,modelUpdater));
}

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, double actionNoise, const Json::Value &options) {
  bool beliefMDP = options.get("beliefMDP",false).asBool();
  std::string updateTypeString = options.get("update","bayesian").asString();
  ModelUpdateType updateType = getModelUpdateType(updateTypeString);

  StateConverter stateConverter = createStateConverter(options);
  return createWorldMDP(rng,dims,beliefMDP,updateType,stateConverter,actionNoise);
}

StateConverter createStateConverter(const Json::Value &options) {
  const Json::Value models = options["models"];
  unsigned int numBeliefs = models.size();
  unsigned int numBins = options.get("numBeliefBins",5).asUInt();
  StateConverter stateConverter(numBeliefs,numBins);
  return stateConverter;
}

///////////////////////////////////////////////////////////////

boost::shared_ptr<UCTEstimator<State_t,Action::Type> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, float lambda, float gamma, float rewardBound, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initialStateActionVisits, float unseenValue, bool theoreticallyCorrectLambda) {
  return boost::shared_ptr<UCTEstimator<State_t,Action::Type> >(new UCTEstimator<State_t,Action::Type>(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda));
}

boost::shared_ptr<ValueEstimator<State_t,Action::Type> > createValueEstimator(boost::shared_ptr<RNG> rng, Action::Type numActions, const Json::Value &options) {
  float lambda = options.get("lambda",0.8).asDouble();
  float gamma = options.get("gamma",0.95).asDouble();
  float unseenValue = options.get("unseenValue",9999999).asDouble();
  float initialValue = options.get("initialValue",0).asDouble();
  float rewardBound = options.get("rewardBound",-1).asDouble();
  float rewardRangePerStep = options.get("rewardRangePerStep",-1).asDouble();

  unsigned int initialStateVisits = options.get("initialStateVisits",0).asUInt();
  unsigned int initialStateActionVisits = options.get("initialStateActionVisits",0).asUInt();
  bool theoreticallyCorrectLambda = options.get("theoreticallyCorrectLambda",true).asBool();

  bool dualUCT = options.get("dualUCT",false).asBool();
  if (dualUCT) {
    StateConverter stateConverter = createStateConverter(options);
    float b = options.get("dualUCTB",0.5).asDouble();
    boost::shared_ptr<UCTEstimator<State_t,Action::Type> > mainValueEstimator = createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
    boost::shared_ptr<UCTEstimator<State_t,Action::Type> > generalValueEstimator = createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
    return boost::shared_ptr<ValueEstimator<State_t,Action::Type> >(new DualUCTEstimator<State_t,Action::Type>(rng,mainValueEstimator,generalValueEstimator,b,stateConverter));
  } else {
    return createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
  }
}

boost::shared_ptr<ValueEstimator<State_t,Action::Type> > createValueEstimator(unsigned int randomSeed, Action::Type numActions, const Json::Value &options) {
  return createValueEstimator(boost::shared_ptr<RNG>(new RNG(randomSeed)),numActions,options);
}

///////////////////////////////////////////////////////////////

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,boost::shared_ptr<ModelUpdater> modelUpdater,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth) {
  return boost::shared_ptr<MCTS<State_t,Action::Type> >(new MCTS<State_t,Action::Type>(model,valueEstimator,modelUpdater,numPlayouts,maxPlanningTime,maxDepth));
}

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<Model<State_t,Action::Type> > model, boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,boost::shared_ptr<ModelUpdater> modelUpdater,const Json::Value &options) {
  unsigned int numPlayouts = options.get("playouts",0).asUInt();
  double maxPlanningTime = options.get("time",0.0).asDouble();
  unsigned int maxDepth = options.get("depth",0).asUInt();

  return createMCTS(model,valueEstimator,modelUpdater,numPlayouts,maxPlanningTime,maxDepth);
}
