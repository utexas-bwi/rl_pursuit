#include "AgentFactory.h"

#include <cstdarg>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <controller/AgentPerturbation.h>
#include <controller/AgentRandom.h>
#include <controller/AgentDummy.h>
#include <controller/PredatorClassifier.h>
#include <controller/PredatorGreedy.h>
#include <controller/PredatorGreedyProbabilistic.h>
#include <controller/PredatorMCTS.h>
#include <controller/PredatorProbabilisticDestinations.h>
#include <controller/PredatorStudentCpp.h>
#include <controller/PredatorStudentPython.h>
#include <controller/PredatorSurround.h>
#include <controller/PredatorSurroundWithPenalties.h>
#include <controller/PredatorTeammateAware.h>
#include <controller/PreyAvoidNeighbor.h>
#include <controller/WorldMDP.h>
#include <planning/UCTEstimator.h>
#include <factory/PlanningFactory.h>
#include <factory/WorldFactory.h>
#include <factory/ClassifierFactory.h>

#define NAME_IN_SET(...) nameInSet(name,__VA_ARGS__,NULL)

bool nameInSet(const std::string &name, ...) {
  va_list vl;
  char *s;
  bool found = false;
  va_start(vl,name);
  while (true) {
    s = va_arg(vl,char *);
    if (s == NULL)
      break;
    else if (name == s) {
      found = true;
      break;
    }
  }
  va_end(vl);
  return found;
}

bool getStudentFromFile(const std::string &filename, std::string &student, unsigned int trialNum) {
  std::ifstream in(filename.c_str());
  if (!in.good())
    return false;
  std::string name;
  for (int i = (int)trialNum; i >= 0; i--) {
    in >> name;
    if (!in.good()) {
      std::cerr << "AgentFactory::getStudentFromFile: ERROR file ended before reaching correct trial num" << std::endl;
      exit(14);
    }
  }
  in.close();
  student = name;
  return true;
}

std::string getStudentForTrial(unsigned int trialNum, const Json::Value &options) {
  std::string student = options.get("student","").asString();
  if (student == "") {
    std::cerr << "createAgent: ERROR: no student type specified" << std::endl;
    exit(3);
  }

  getStudentFromFile(student,student,trialNum);
  return student;
}

void getAvailableStudents(const std::string &filename, std::set<std::string> &students) {
  std::ifstream in(filename.c_str());
  assert(in.good());
  std::string name;
  while (true) {
    in >> name;
    if (!in.good())
      break;
    students.insert(name);
  }
  in.close();
}

AgentPerturbation::Perturbation getPerturbation(unsigned int trialNum, int predatorInd, const Json::Value &options) {
  AgentPerturbation::Perturbation perturbation;
  std::string type;
  std::string filename = options.get("filename","").asString();
  if (filename.size() > 0) {
    getPerturbationFromFile(trialNum,predatorInd,filename,perturbation.amount,type);
  } else {
    type = options.get("type","").asString();
    perturbation.amount = options.get("amount",-1).asDouble();
  }
  if (type == "noop")
    perturbation.type = AgentPerturbation::NOOP;
  else if (nameInSet(type,"rand","random",NULL))
    perturbation.type = AgentPerturbation::RANDOM;
  else {
    std::cerr << "getPerturbation: ERROR: Unknown perturbation type: " << type << std::endl;
    exit(56);
  }
  if ((perturbation.amount < 0) || (perturbation.amount > 1.0)) {
    std::cerr << "getPerturbation: ERROR: Invalid perturbation amount: " << perturbation.amount << std::endl;
    exit(57);
  }
  return perturbation;
}

void getPerturbationFromFile(unsigned int trialNum, int predatorInd, const std::string &filename, float &amount, std::string &typeStr) {
  std::ifstream in(filename.c_str());
  unsigned int NUM_PREDATORS = 4;
  for (unsigned int i = 0; i < trialNum + 1; i++) {
    for (unsigned int j = 0; j < NUM_PREDATORS; j++) {
      in >> typeStr;
      in >> amount;
      //std::cout << typeStr << " " << amount << std::endl;
      if (!in.good()) {
        std::cerr << "getPerturbation: ERROR: file ended too soon for: " << filename << " " << trialNum << " " << predatorInd << std::endl;
        exit(58);
      }
      if ((int)j == predatorInd) {
        in.close();
        return;
      }
    }
  }
  in.close();

  std::cerr << "getPerturbationFromFile: ERROR: HOW DID I GET HERE: " << filename << " " << trialNum << " " << predatorInd << std::endl;
  exit(59);
}

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent) {
  typedef boost::shared_ptr<Agent> ptr;
  
  boost::to_lower(name);
  if (NAME_IN_SET("prey","preyrandom","random","ra"))
    return ptr(new AgentRandom(rng,dims));
  if (NAME_IN_SET("preyavoidneighbor","avoidneighbor","preyavoid","avoid", "an"))
    return ptr(new PreyAvoidNeighbor(rng,dims));
  else if (NAME_IN_SET("greedy","gr"))
    return ptr(new PredatorGreedy(rng,dims));
  else if (NAME_IN_SET("greedyprobabilistic","greedyprob","gp"))
    return ptr(new PredatorGreedyProbabilistic(rng,dims));
  else if (NAME_IN_SET("probabilisticdestinations","probdests","pd"))
    return ptr(new PredatorProbabilisticDestinations(rng,dims));
  else if (NAME_IN_SET("teammate-aware","ta"))
    return ptr(new PredatorTeammateAware(rng,dims));
  else if (NAME_IN_SET("dummy")) {
    Action::Type action = (Action::Type)options.get("action",Action::NOOP).asInt();
    return ptr(new AgentDummy(rng,dims,action));
  } else if (NAME_IN_SET("perturb","perturbation")) {
    AgentPerturbation::Perturbation perturbation = getPerturbation(trialNum,predatorInd,options);
    std::string baseAgentName = options.get("base","").asString();
    ptr origAgent = createAgent(rng,dims,baseAgentName,trialNum,predatorInd,options["baseOptions"],rootOptions,baseAgent);
    return ptr(new AgentPerturbation(rng,dims,origAgent,perturbation));
  } else if (NAME_IN_SET("mixed")) {
    Json::Value types = options["types"];
    std::string typeName;
    if (types.size() > 0) {
      typeName = types[predatorInd].asString();
    } else {
      std::vector<std::string> typeNames;
      typeNames.push_back("gr");
      typeNames.push_back("ta");
      typeNames.push_back("gp");
      typeNames.push_back("gp");
      int ind = rng->randomInt(typeNames.size());
      typeName = typeNames[ind];
    }
    return createAgent(rng,dims,typeName,trialNum,predatorInd,options,rootOptions);
  }
  else if (NAME_IN_SET("surround","surround","su"))
    return ptr(new PredatorSurround(rng,dims));
  else if (NAME_IN_SET("surroundpenalties","surround-penalties","sp")) {
    bool outputPenaltyMode = options.get("outputPenaltyMode",false).asBool();
    return ptr(new PredatorSurroundWithPenalties(rng,dims,outputPenaltyMode));
  } else if (NAME_IN_SET("dt","decision","decisiontree","decision-tree","classifier","class")) {
    std::string filename = options.get("filename","").asString();
    int trainingPeriod = options.get("trainingPeriod",-1).asInt();
    bool trainIncremental = options.get("trainIncremental",true).asBool();
    boost::shared_ptr<Classifier> classifier;
    if (baseAgent.get() != NULL) {
      // get the classifier of the base agent, if it's of the correct type
      boost::shared_ptr<PredatorClassifier> basePred = boost::static_pointer_cast<PredatorClassifier>(baseAgent);
      if (basePred.get() != NULL) {
        std::cout << "SHARING CLASSIFIER" << std::endl;
        classifier = basePred->getClassifier();
        basePred->setPreventTraining(true);
      }
    }
    if (classifier.get() == NULL)
      classifier = createClassifier(filename,options);
    classifier->setRNG(rng);

    return ptr(new PredatorClassifier(rng,dims,classifier,filename,trainingPeriod,trainIncremental));
  } else if (NAME_IN_SET("student")) {
    std::string student = getStudentForTrial(trialNum,options);
    if ((predatorInd < 0) || (predatorInd >= 4)) {
      std::cerr << "createAgent: ERROR: bad predator ind specified for student: " << student << std::endl;
      exit(3);
    }
    if (PredatorStudentCpp::handlesStudent(student))
      return ptr(new PredatorStudentCpp(rng,dims,student,predatorInd));
    else
      return ptr(new PredatorStudentPython(rng,dims,student,predatorInd));
  } else if (NAME_IN_SET("mcts","uct")) {
    Json::Value plannerOptions = rootOptions["planner"];
    // process the depth if necessary
    unsigned int depth = plannerOptions.get("depth",0).asUInt();
    if (depth == 0) {
      unsigned int depthFactor = plannerOptions.get("depthFactor",0).asUInt();
      plannerOptions["depth"] = depthFactor * (dims.x + dims.y);
    }

    double actionNoise = getActionNoise(rootOptions);
    bool centerPrey = getCenterPrey(rootOptions);
    
    // create the mdp
    boost::shared_ptr<WorldMDP> mdp = createWorldMDP(rng,dims,actionNoise,centerPrey,plannerOptions);
    // create the model updater
    boost::shared_ptr<ModelUpdater> modelUpdater = createModelUpdater(rng,mdp,dims,trialNum,predatorInd,plannerOptions); // predatorInd should be the replacement ind for the model
    // create the value estimator
    boost::shared_ptr<ValueEstimator<State_t,Action::Type> > estimator = createValueEstimator(rng->randomUInt(),Action::NUM_ACTIONS,plannerOptions);
    // create the planner
    boost::shared_ptr<MCTS<State_t,Action::Type> > mcts = createMCTS(estimator,modelUpdater,plannerOptions);

    return ptr(new PredatorMCTS(rng,dims,mcts,modelUpdater));
  } else {
    std::cerr << "createAgent: unknown agent name: " << name << std::endl;
    exit(25);
  }
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createAgent(rng,dims,name,trialNum,predatorInd,options,rootOptions,baseAgent);
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, unsigned int trialNum, int predatorInd, const Json::Value &options, const Json::Value &rootOptions, boost::shared_ptr<Agent> baseAgent) {
  std::string name = options.get("behavior","NONE").asString();
  if (name == "NONE") {
    std::cerr << "createAgent: WARNING: no agent type specified, using random" << std::endl;
    name = "random";
  }

  return createAgent(randomSeed,dims,name,trialNum,predatorInd,options,rootOptions,baseAgent);
}
