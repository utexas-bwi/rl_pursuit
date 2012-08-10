/*
File: FeatureExtractor.cpp
Author: Samuel Barrett
Description: extracts a set of features of the agents
Created:  2011-10-28
Modified: 2011-12-10
*/

#include "FeatureExtractor.h"
#include <boost/lexical_cast.hpp>
#include <factory/AgentFactory.h>

const unsigned int FeatureExtractor::HISTORY_SIZE = 2;
const bool FeatureExtractor::USE_ALL_AGENTS_HISTORY = false;

//#define FEATURE_EXTRACTOR_TIMING

#ifdef FEATURE_EXTRACTOR_TIMING

#define TIC(s) FEATURE_EXTRACTOR_TIMING_ ## s.tic()
#define TOC(s) FEATURE_EXTRACTOR_TIMING_ ## s.toc()
#define GETTIME(s) FEATURE_EXTRACTOR_TIMING_ ## s.get()
#define OUTPUT(s) #s << "(" << GETTIME(s) << ") " 
#define MAKE(s) Timer FEATURE_EXTRACTOR_TIMING_ ## s

MAKE(total);
MAKE(pos);
MAKE(derived);
MAKE(actions);
MAKE(history);
MAKE(historycalc);
MAKE(historyupdate);
MAKE(historydiff);
MAKE(historyaction);
MAKE(historyuncenter);

#else

#define TIC(s) ((void) 0)
#define TOC(s) ((void) 0)
#define GETTIME(s) ((void) 0)
#define OUTPUT(s) ((void) 0)

#endif

FeatureExtractorHistory::FeatureExtractorHistory():
  initialized(false),
  actionHistory()
{
}

void FeatureExtractorHistory::reset() {
  initialized = false;
  actionHistory.clear();
}

FeatureExtractor::FeatureExtractor(const Point2D &dims):
  dims(dims)
{
}

void FeatureExtractor::addFeatureAgent(const std::string &key, const std::string &name) {
  FeatureAgent featureAgent;
  featureAgent.name = key;
  featureAgent.agent = createAgent(0,dims,name,0,0,Json::Value(),Json::Value()); // the rng, trialNum, and predatorInd don't matter here
  featureAgents.push_back(featureAgent);
}

InstancePtr FeatureExtractor::extract(const Observation &obs, FeatureExtractorHistory &history) {
  TIC(total);
  assert(obs.preyInd == 0);
  InstancePtr instance(new Instance);
  
  TIC(pos);
  setFeature(instance,FeatureType::PredInd,obs.myInd - 1);
  // positions of agents
  for (unsigned int i = 0; i < obs.positions.size(); i++) {
    Point2D diff = getDifferenceToPoint(dims,obs.myPos(),obs.positions[i]);
    unsigned int key = FeatureType::Prey_dx + 2 * i;
    setFeature(instance,key,diff.x);
    setFeature(instance,key+1,diff.y);
  }
  TOC(pos);
  // derived features
  TIC(derived);
  bool next2prey = false;
  for (unsigned int a = 0; a < Action::NUM_NEIGHBORS; a++) {
    Point2D pos = movePosition(dims,obs.myPos(),(Action::Type)a);
    bool occupied = false;
    for (unsigned int i = 0; i < obs.positions.size(); i++) {
      if (i == obs.myInd)
        continue;
      if (obs.positions[i] == pos) {
        occupied = true;
        if (i == 0)
          next2prey = true;
        break;
      }
    }
    setFeature(instance,FeatureType::Occupied_0 + a, occupied);
  }
  setFeature(instance,FeatureType::NextToPrey,next2prey);
  TOC(derived);
  // actions predicted by models
  TIC(actions);
  // not currently supported
  //ActionProbs actionProbs;
  for (std::vector<FeatureAgent>::iterator it = featureAgents.begin(); it != featureAgents.end(); it++) {
    std::cerr << "FeatureExtractor can't handle featureAgents" << std::endl;
    exit(58);
    //actionProbs = it->agent->step(obs);
    //ADD_KEY(it->name + ".des");
    //setFeature(instance,actionProbs.maxAction());
  }
  TOC(actions);
  // update the history
  TIC(history);
  updateHistory(obs,history);
  // add the history features
  TIC(historyupdate);
  Action::Type action;
  for (unsigned int j = 0; j < HISTORY_SIZE; j++) {
    if (j < history.actionHistory[obs.myInd].size())
      action = history.actionHistory[obs.myInd][j];
    else
      action = Action::NUM_ACTIONS;
    setFeature(instance,FeatureType::MyHistoricalAction_0 + j,action);
  }
  TOC(historyupdate);
/*
  for (unsigned int agentInd = 0; agentInd < obs.positions.size(); agentInd++) {
    for (unsigned int j = 0; j < HISTORY_SIZE; j++) {
      if (j < history.actionHistory[agentInd].size())
        action = history.actionHistory[agentInd][j];
      else
        action = Action::NUM_ACTIONS;
      if (USE_ALL_AGENTS_HISTORY) {
        std::cerr << "FeatureExtractor can't handle all agents history" << std::endl;
        exit(58);
        //ADD_KEY("HistoricalAction" + boost::lexical_cast<std::string>(agentInd) + "." + boost::lexical_cast<std::string>(j));
        //setFeature(instance,action);
      }

      if (agentInd == obs.myInd) {
        setFeature(instance,FeatureType::MyHistoricalAction_0 + j,action);
      }

    }
  }
*/
  TOC(history);

  instance->weight = 1.0;
  TOC(total);
  //std::cout << "instance: " << *instance << std::endl;
  return instance;
}

void FeatureExtractor::updateHistory(const Observation &obs, FeatureExtractorHistory &history) {
  std::vector<Action::Type> observedActions;
  if (history.initialized) {
    TIC(historycalc);
    calcObservedActions(history.obs,obs,observedActions);
    TOC(historycalc);
  } else {
    //std::cout << "no hist " << obs << std::endl;
    for (unsigned int i = 0; i < obs.positions.size(); i++) {
      history.actionHistory.push_back(boost::circular_buffer<Action::Type>(HISTORY_SIZE));
      observedActions.push_back(Action::NUM_ACTIONS);
    }
  }
  for (unsigned int agentInd = 0; agentInd < obs.positions.size(); agentInd++) {
    history.actionHistory[agentInd].push_front(observedActions[agentInd]);
  }
  history.initialized = true;
  history.obs = obs;
}

void FeatureExtractor::calcObservedActions(Observation prevObs, Observation obs, std::vector<Action::Type> &actions) {
  actions.resize(prevObs.positions.size());
  TIC(historyuncenter);
  prevObs.uncenterPrey(dims);
  obs.uncenterPrey(dims);
  TOC(historyuncenter);
  //std::cout << prevObs << " " << obs << std::endl << std::flush;
  bool prevCapture = obs.didPreyMoveIllegally(dims,prevObs.absPrey);
  for (unsigned int i = 0; i < prevObs.positions.size(); i++) {
    // skip if the prey was captured last step
    if (prevCapture && ((int)i == obs.preyInd)) {
      actions[i] = Action::NUM_ACTIONS;
      continue;
    }
    TIC(historydiff);
    Point2D diff = getDifferenceToPoint(dims,prevObs.positions[i],obs.positions[i]);
    TOC(historydiff);
    TIC(historyaction);
    //actions.push_back(getAction(diff));
    actions[i] = getAction(diff);
    TOC(historyaction);
  }
}

//void FeatureExtractor::setFeature(InstancePtr &instance, const std::string &key, float val) {
  //(*instance)[key] = val;
//}

void FeatureExtractor::printTimes() {
#ifdef FEATURE_EXTRACTOR_TIMING
  std::cout << "FeatureExtractor Timings: " << OUTPUT(total) << OUTPUT(pos) << OUTPUT(derived) << OUTPUT(actions) << OUTPUT(history) << std::endl;
  std::cout << "  " << OUTPUT(history) << ":" << OUTPUT(historycalc) << OUTPUT(historyupdate) << OUTPUT(historydiff) << OUTPUT(historyaction) << OUTPUT(historyuncenter) << std::endl;
#endif
}
