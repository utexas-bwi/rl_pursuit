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

// MODEL UPDATER
boost::shared_ptr<ModelUpdaterBayes> createModelUpdaterBayes(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<std::vector<boost::shared_ptr<Agent> > > &modelList, const std::vector<double> &modelProbs, const std::vector<std::string> &modelDescriptions, ModelUpdateType updateType) {
    return boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(rng,mdp,modelList,modelProbs,modelDescriptions,updateType));
}

boost::shared_ptr<ModelUpdater> createModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, boost::shared_ptr<Agent> adhocAgent, const Point2D &dims, int replacementInd, const Json::Value &options) {
  // create the agents
  const Json::Value models = options["models"];
  std::vector<std::vector<boost::shared_ptr<Agent> > > modelList(models.size());
  std::vector<double> modelProbs;
  std::vector<std::string> modelDescriptions;
  for (unsigned int i = 0; i < models.size(); i++) {
    modelProbs.push_back(models[i].get("prob",1.0).asDouble());
    modelDescriptions.push_back(models[i].get("desc","NO DESCRIPTION").asString());

    std::vector<AgentModel> agentModels;
    createAgentControllersAndModels(rng,dims,0,replacementInd,models[i],adhocAgent,modelList[i],agentModels);
    
    // add the first set of agents to the world
    if (i == 0) {
      for (unsigned int j = 0; j < modelList[i].size(); j++)
        mdp->addAgent(agentModels[j].type,modelList[i][j]);
    }
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

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, bool beliefMDP, unsigned int numBeliefs, unsigned int numBins, ModelUpdateType updateType) {
  // create the world model and controller
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  boost::shared_ptr<World> controller = createWorld(rng->randomUInt(),model);
  // create the dummy agent for the ad hoc agent
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(boost::shared_ptr<RNG>(new RNG(rng->randomUInt())),dims));
  boost::shared_ptr<WorldMDP> mdp(new WorldMDP(rng,model,controller,adhocAgent));
  if (! beliefMDP)
    return mdp;

  //std::cout << "HERE" << std::endl;
  //controller = createWorldAgents(boost::shared_ptr<RNG>(new RNG(rng->randomUInt())),controller,0,Json::Value());
  //std::cout << "HERE2" << std::endl;
 
  boost::shared_ptr<ModelUpdaterBayes> modelUpdater = createModelUpdaterBayes(rng,mdp,std::vector<std::vector<boost::shared_ptr<Agent> > >(),std::vector<double>(),std::vector<std::string>(),updateType);
  StateConverter stateConverter(numBeliefs,numBins);
  return boost::shared_ptr<WorldMDP>(new WorldBeliefMDP(rng,model,controller,adhocAgent,stateConverter,modelUpdater));
}

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, const Json::Value &options) {
  bool beliefMDP = options.get("beliefMDP",false).asBool();
  const Json::Value models = options["models"];
  unsigned int numBeliefs = models.size();
  unsigned int numBins = options.get("numBeliefBins",5).asUInt();
  std::string updateTypeString = options.get("update","bayesian").asString();
  ModelUpdateType updateType = getModelUpdateType(updateTypeString);

  return createWorldMDP(rng,dims,beliefMDP,numBeliefs,numBins,updateType);
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

  //bool dualUCT = options.get("dualUCT",false).asBool();
  //if (dualUCT) {
    //float b = options.get("dualUCTB",0.5).asDouble();
    //boost::shared_ptr<UCTEstimator<State_t,Action::Type> > mainValueEstimator = createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
    //boost::shared_ptr<UCTEstimator<State_t,Action::Type> > generalValueEstimator = createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
    //return boost::shared_ptr<ValueEstimator<State_t,Action::Type> >(new DualUCTEstimator<State_t,Action::Type>(rng,mainValueEstimator,generalValueEstimator,b,&convertBeliefStateToGeneralState));
  //} else {
    return createUCTEstimator(rng,numActions,lambda,gamma,rewardBound,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue,theoreticallyCorrectLambda);
//}
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
