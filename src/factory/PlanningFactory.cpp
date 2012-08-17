/*
File: PlanningFactory.cpp
Author: Samuel Barrett
Description: generates objects for planning
Created:  2011-08-24
Modified: 2011-10-02
*/

#include "PlanningFactory.h"
#include <boost/algorithm/string.hpp>
#include <fstream>
#include <boost/foreach.hpp>
//#include <controller/WorldSilverMDP.h>
//#include <controller/WorldSilverWeightedMDP.h>
#include <controller/ModelUpdaterBayes.h>
#include <controller/ModelUpdaterSilver.h>
#include <planning/DualUCTEstimator.h>
#include "WorldFactory.h"
#include <controller/State.h>
#include <controller/WorldBeliefMDP.h>

boost::shared_ptr<RNG> makeRNG(unsigned int seed) {
  return boost::shared_ptr<RNG>(new RNG(seed));
}

ReplaceDataStudent::ReplaceDataStudent(const std::string &searchStr, const std::set<std::string> &dataStudents):
  searchStr(searchStr),
  dataStudents(dataStudents)
{
}

void ReplaceDataStudent::operator() (Json::Value &value) {
  if (!value.isString())
    return;
  std::string str = value.asString();
  size_t pos = str.find(searchStr);
  if (pos == std::string::npos)
    return;
  std::string repl;
  BOOST_FOREACH(std::string student, dataStudents) {
    if (repl.size() > 0)
      repl += ";";
    std::string temp(str); // need to make copy because replace is in place
    temp.replace(pos,searchStr.size(),student);
    repl += temp;
  }
  value = Json::Value(repl);
}

// MODEL UPDATER
boost::shared_ptr<ModelUpdaterBayes> createModelUpdaterBayes(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models, const ModelUpdaterBayes::Params &params) {
    return boost::shared_ptr<ModelUpdaterBayes>(new ModelUpdaterBayes(rng,models,params));
}

boost::shared_ptr<ModelUpdater> createModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &options) {
  // create the agents
  const Json::Value &models = options["models"];
  std::vector<ModelInfo> modelList;
  std::string currentStudent = options.get("student","UNKNOWN_STUDENT").asString();

  for (unsigned int i = 0; i < models.size(); i++) {
    bool modelPerStudent = models[i].get("modelPerStudent",false).asBool();
    bool dataPerStudent = models[i].get("dataPerStudent",false).asBool();
    bool includeCurrentStudent = models[i].get("includeCurrentStudent",true).asBool();
    bool randomForest = models[i].get("randomForest",false).asBool();
    unsigned int numTrees = models[i].get("numTrees",10).asUInt();
    bool useListModels = models[i].get("useModelList",false).asBool();

    if (useListModels) {
      createListModels(rng,mdp,dims,trialNum,replacementInd,currentStudent,models[i],modelList);
      continue;
    }

    if (!randomForest)
      numTrees = 1;
    std::string pred = models[i].get("predator","UNKNOWN_PRED").asString();
    if (pred == "classifier")
      std::cout << "dataPerStudent: " << std::boolalpha << dataPerStudent << std::endl;

    std::set<std::string> students;
    if (modelPerStudent) {
      getAvailableStudents(options["students"].asString(),students);
      if (!includeCurrentStudent) {
        students.erase(currentStudent);
      }
    } else {
      students.insert(currentStudent);
    }

    Json::Value modelOptionsV1(models[i]);
    if (dataPerStudent) {
      std::set<std::string> dataStudents;
      getAvailableStudents(options["students"].asString(),dataStudents);
      if (!includeCurrentStudent)
        dataStudents.erase(currentStudent);
      jsonReplaceStrings(modelOptionsV1,ReplaceDataStudent("$(DATA_STUDENT)",dataStudents));
    }
    
    // iterate through the considered students
    for (std::set<std::string>::iterator it = students.begin(); it != students.end(); it++) {
      for (unsigned int treeInd = 0; treeInd < numTrees; treeInd++) {
        Json::Value modelOptions(modelOptionsV1);
        std::map<std::string,std::string> reps;
        std::string modelStudent = *it;
        if (randomForest)
          modelStudent += "-" + boost::lexical_cast<std::string>(treeInd);
        reps["$(MODEL_STUDENT)"] = modelStudent;
        jsonReplaceStrings(modelOptions,reps);
        
        createAndAddModel(rng,mdp,dims,trialNum,replacementInd,modelOptions,modelList);
      }
    }
  }

  boost::shared_ptr<ModelUpdater> ptr;
  if (options.get("silver",false).asBool()) {
    // make a silver updater
    ModelUpdaterSilver::Params p;
    p.fromJson(options);
    ptr = boost::shared_ptr<ModelUpdaterSilver>(new ModelUpdaterSilver(rng,modelList,p));
  } else {
    // make a bayes updater
    //std::string updateTypeString = options.get("update","bayesian").asString();
    //ModelUpdateType updateType = getModelUpdateType(updateTypeString);
    //bool allowRemovingModels = options.get("allowRemovingModels",true).asBool();
    //float minModelProb = options.get("minModelProb",0.001).asDouble();
    //ptr = createModelUpdaterBayes(rng,modelList,updateType,allowRemovingModels,minModelProb);
    ModelUpdaterBayes::Params p;
    p.fromJson(options);

    ptr = createModelUpdaterBayes(rng,modelList,p);
  }
  // optionally enable output
  std::string modelOutput = options.get("modelOutputFile","").asString();
  if (modelOutput != "") {
    std::cout << "ENABLING OUTPUT: " << modelOutput << std::endl;
    boost::shared_ptr<std::ostream> out(new std::ofstream(modelOutput.c_str()));
    ptr->enableOutput(out);
  }
  // optionally enable precision output
  std::string modelPrecisionOutput = options.get("modelPrecisionOutputFile","").asString();
  if (modelOutput != "") {
    std::cout << "ENABLING PRECISION OUTPUT: " << modelPrecisionOutput << std::endl;
    boost::shared_ptr<std::ostream> outPrecision(new std::ofstream(modelPrecisionOutput.c_str()));
    ptr->enablePrecisionOutput(outPrecision);
  }
  return ptr;
}

void createAndAddModel(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const Point2D &dims, unsigned int trialNum, int replacementInd, const Json::Value &modelOptions, std::vector<ModelInfo> &modelList) {
  double prob = modelOptions.get("prob",1.0).asDouble();
  std::string desc = modelOptions.get("desc","NO DESCRIPTION").asString();
  //bool caching = modelOptions.get("cache",false).asBool();

  std::vector<AgentModel> agentModels;
  std::vector<AgentPtr> agents;
  boost::shared_ptr<WorldMDP> newMDP = mdp->clone();
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(rng,dims));
  newMDP->setAdhocAgent(adhocAgent);
  
  createAgentControllersAndModels(rng,dims,trialNum,replacementInd,modelOptions,adhocAgent,agents,agentModels);
  newMDP->addAgents(agentModels,agents);
  //newMDP->setCaching(caching);
  //newMDP->resetCache();

  modelList.push_back(ModelInfo(newMDP,desc,prob));
}

void createListModels(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const Point2D &dims, unsigned int trialNum, int replacementInd, const std::string &student, const Json::Value &options, std::vector<ModelInfo> &modelList) {
  std::cout << "creating list models" << std::endl;
  std::string modelListFile = options.get("modelListFile","").asString();
  std::string modelDir = options.get("modelDir","").asString();
  if (modelDir == "") {
    std::cerr << "Must specify modelDir" << std::endl;
    exit(97);
  }
  std::vector<std::string> models;
  if (!readModelsFromListFile(modelListFile,student,models) || (models.size() == 0)) {
    std::cerr << "Error reading list models for " << student << " from file: " << modelListFile << std::endl;
    exit(98);
  }
  
  for (unsigned int i = 0; i < models.size(); i++) {
    std::cout << "reading model " << i << ": " << models[i] << std::endl;
    Json::Value modelOptions;
    std::string modelFile = modelDir + "/" + models[i] + ".json";
    if (!readJson(modelFile,modelOptions)) {
      std::cerr << "Error reading in model " << modelFile << " at location: " << modelFile << std::endl;
      exit(99);
    }
    createAndAddModel(rng,mdp,dims,trialNum,replacementInd,modelOptions,modelList);
  }
}

bool readModelsFromListFile(const std::string &filename, const std::string &student, std::vector<std::string> &modelNames) {
  std::ifstream in(filename.c_str());
  std::string line;

  while (in.good()) {
    modelNames.clear();
    std::getline(in,line);
    if (!in.good())
      break;
    boost::split(modelNames,line,boost::is_any_of(","));
    if (modelNames[0] != student)
      continue;
    modelNames.erase(modelNames.begin());
    return true;
  }
  return false;
}


boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, bool usePreySymmetry, bool beliefMDP, ModelUpdateType_t /*updateType*/, const StateConverter &/*stateConverter*/, double actionNoise, bool centerPrey) {
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
  boost::shared_ptr<World> controller = createWorld(rngWorld1,model,actionNoise, centerPrey);
  // create the dummy agent for the ad hoc agent
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(rngDummy,dims));

  boost::shared_ptr<WorldMDP> mdp(new WorldMDP(rngMDP1,model,controller,adhocAgent,usePreySymmetry));
  if (! beliefMDP)
    return mdp;

  //model = createWorldModel(dims);
  controller = createWorld(rngWorld2,model,actionNoise,centerPrey);

  //std::cout << "HERE" << std::endl;
  //controller = createWorldAgents(rngOther,controller,0,Json::Value());
  //std::cout << "HERE2" << std::endl;
 
  // TODO fix belief mdp
  assert(false);
  //boost::shared_ptr<ModelUpdaterBayes> modelUpdater = createModelUpdaterBayes(rngModelUpdater,mdp,std::vector<std::vector<boost::shared_ptr<Agent> > >(),std::vector<double>(),std::vector<std::string>(),updateType);
  //return boost::shared_ptr<WorldMDP>(new WorldBeliefMDP(rngMDP2,model,controller,adhocAgent,usePreySymmetry,stateConverter,modelUpdater));
}

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, double actionNoise, bool centerPrey, const Json::Value &options) {
  bool usePreySymmetry = options.get("preySymmetry",true).asBool();
  bool beliefMDP = options.get("beliefMDP",false).asBool();
  std::string updateTypeString = options.get("update","bayesian").asString();
  ModelUpdateType_t updateType = ModelUpdateType::fromName(updateTypeString);

  StateConverter stateConverter = createStateConverter(options);
  return createWorldMDP(rng,dims,usePreySymmetry,beliefMDP,updateType,stateConverter,actionNoise, centerPrey);
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

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,boost::shared_ptr<ModelUpdater> modelUpdater,unsigned int numPlayouts, double maxPlanningTime, unsigned int maxDepth, int pruningMemorySize) {
  return boost::shared_ptr<MCTS<State_t,Action::Type> >(new MCTS<State_t,Action::Type>(valueEstimator,modelUpdater,numPlayouts,maxPlanningTime,maxDepth,pruningMemorySize));
}

boost::shared_ptr<MCTS<State_t,Action::Type> > createMCTS(boost::shared_ptr<ValueEstimator<State_t,Action::Type> > valueEstimator,boost::shared_ptr<ModelUpdater> modelUpdater,const Json::Value &options) {
  unsigned int numPlayouts = options.get("playouts",0).asUInt();
  double maxPlanningTime = options.get("time",0.0).asDouble();
  unsigned int maxDepth = options.get("depth",0).asUInt();
  int pruningMemorySize = options.get("pruningMemory",-1).asInt();

  return createMCTS(valueEstimator,modelUpdater,numPlayouts,maxPlanningTime,maxDepth,pruningMemorySize);
}
