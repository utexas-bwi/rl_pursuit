/*
File: FeatureExtractor.cpp
Author: Samuel Barrett
Description: extracts a set of features of the agents
Created:  2011-10-28
Modified: 2011-10-28
*/

#include "FeatureExtractor.h"
#include <boost/lexical_cast.hpp>
#include <factory/AgentFactory.h>

const unsigned int FeatureExtractor::HISTORY_SIZE = 2;

FeatureExtractorHistory::FeatureExtractorHistory():
  actionHistory(FeatureExtractor::HISTORY_SIZE)
{
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
  assert(obs.preyInd == 0);
  InstancePtr instance(new Instance);
  
  setFeature(instance,"PredInd",obs.myInd - 1);
  // positions of agents
  for (unsigned int i = 0; i < obs.positions.size(); i++) {
    Point2D diff = getDifferenceToPoint(dims,obs.myPos(),obs.positions[i]);
    std::string key;
    if (i == 0)
      key = "Prey";
    else
      key = "Pred" + boost::lexical_cast<std::string>(i-1);
    setFeature(instance,key + ".dx",diff.x);
    setFeature(instance,key + ".dy",diff.y);
  }
  // derived features
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
    std::string key = "Occupied." + boost::lexical_cast<std::string>(a);
    setFeature(instance,key,occupied);
  }
  setFeature(instance,"NextToPrey",next2prey);
  // actions predicted by models
  ActionProbs actionProbs;
  for (std::vector<FeatureAgent>::iterator it = featureAgents.begin(); it != featureAgents.end(); it++) {
    actionProbs = it->agent->step(obs);
    setFeature(instance,it->name + ".des",actionProbs.maxAction());
  }
  // history features and update the history
  Action::Type a;
  for (unsigned int j = 0; j < HISTORY_SIZE; j++) {
    if (j < history.actionHistory.size())
      a = history.actionHistory[j];
    else
      a = Action::NUM_ACTIONS;
    std::string key = "HistoricalAction." + boost::lexical_cast<std::string>(j);
    setFeature(instance,key,a);
    // update it
    history.actionHistory.push_front();
  }

  instance->weight = 1.0;
  return instance;
}

void FeatureExtractor::setFeature(InstancePtr &instance, const std::string &key, float val) {
  (*instance)[key] = val;
}
