#include "AgentFactory.h"

#include <cstdarg>
#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <controller/AgentRandom.h>
#include <controller/AgentDummy.h>
#include <controller/PredatorDecisionTree.h>
#include <controller/PredatorGreedy.h>
#include <controller/PredatorGreedyProbabilistic.h>
#include <controller/PredatorMCTS.h>
#include <controller/PredatorProbabilisticDestinations.h>
#include <controller/PredatorStudentCpp.h>
#include <controller/PredatorStudentPython.h>
#include <controller/PredatorTeammateAware.h>
#include <controller/WorldMDP.h>
#include <planning/UCTEstimator.h>
#include <factory/PlanningFactory.h>

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
  for (; trialNum > 0; trialNum--) {
    in >> name;
    if (!in.good()) {
      std::cerr << "AgentFactory::getStudentFromFile: ERROR file ended before reaching correct trial num" << std::endl;
      in.close();
      return false;
    }
  }
  in.close();
  student = name;
  return true;
}

boost::shared_ptr<Agent> createAgent(boost::shared_ptr<RNG> rng, const Point2D &dims, std::string name, unsigned int randomNum, const Json::Value &options, const Json::Value &rootOptions) {
  typedef boost::shared_ptr<Agent> ptr;
  
  boost::to_lower(name);
  if (NAME_IN_SET("prey","preyrandom","random"))
    return ptr(new AgentRandom(rng,dims));
  else if (NAME_IN_SET("greedy","gr"))
    return ptr(new PredatorGreedy(rng,dims));
  else if (NAME_IN_SET("greedyprobabilistic","greedyprob","gp"))
    return ptr(new PredatorGreedyProbabilistic(rng,dims));
  else if (NAME_IN_SET("probabilisticdestinations","probdests","pd"))
    return ptr(new PredatorProbabilisticDestinations(rng,dims));
  else if (NAME_IN_SET("teammate-aware","ta"))
    return ptr(new PredatorTeammateAware(rng,dims));
  else if (NAME_IN_SET("dummy"))
    return ptr(new AgentDummy(rng,dims));
  else if (NAME_IN_SET("dt","decision","decisiontree","decision-tree")) {
    std::string filename = options.get("filename","").asString();
    std::string sizeId = "$(SIZE)";
    size_t ind = filename.find(sizeId);
    if (ind != std::string::npos) {
      std::string size = boost::lexical_cast<std::string>(dims.x) + "x" + boost::lexical_cast<std::string>(dims.y);
      filename.replace(ind,sizeId.size(),size);
    }
    return ptr(new PredatorDecisionTree(rng,dims,filename));
  } else if (NAME_IN_SET("student")) {
    std::string student = options.get("student","").asString();
    if (student == "") {
      std::cerr << "createAgent: ERROR: no student type specified" << std::endl;
      exit(3);
    }

    getStudentFromFile(student,student,trialNum);

    int predatorInd = options.get("predatorInd",-1).asInt();
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
    
    // create the mdp
    boost::shared_ptr<WorldMDP> mdp = createWorldMDP(rng,dims);
    // create the model updater
    boost::shared_ptr<ModelUpdater> modelUpdater = createModelUpdater(rng,mdp,mdp->getAdhocAgent(),dims,plannerOptions);
    // create the value estimator
    boost::shared_ptr<UCTEstimator<State_t,Action::Type> > uct = createUCTEstimator(rng->randomUInt(),Action::NUM_ACTIONS,plannerOptions);
    // create the planner
    boost::shared_ptr<MCTS<State_t,Action::Type> > mcts = createMCTS(mdp,uct,modelUpdater,plannerOptions);

    return ptr(new PredatorMCTS(rng,dims,mcts,mdp,modelUpdater));
  } else {
    std::cerr << "createAgent: unknown agent name: " << name << std::endl;
    assert(false);
  }
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, std::string name, unsigned int randomNum, const Json::Value &options, const Json::Value &rootOptions) {
  boost::shared_ptr<RNG> rng(new RNG(randomSeed));
  return createAgent(rng,dims,name,randomNum,options,rootOptions);
}

boost::shared_ptr<Agent> createAgent(unsigned int randomSeed, const Point2D &dims, unsigned int randomNum, const Json::Value &options, const Json::Value &rootOptions) {
  std::string name = options.get("behavior","NONE").asString();
  if (name == "NONE") {
    std::cerr << "createAgent: WARNING: no agent type specified, using random" << std::endl;
    name = "random";
  }

  return createAgent(randomSeed,dims,name,randomNum,options,rootOptions);
}
