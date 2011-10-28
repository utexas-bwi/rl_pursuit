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

FeatureExtractor::FeatureExtractor(const Point2D &dims):
  dims(dims)
{
}

void FeatureExtractor::addFeatureAgent(const std::string &key, const std::string &name) {
  featureAgents[key] = createAgent(0,dims,name,0,0,Json::Value(),Json::Value()); // the rng, trialNum, and predatorInd don't matter here
}

void FeatureExtractor::extract(const Observation &obs, Features &features) {
  assert(obs.preyInd == 0);
  
  unsigned int predInd = obs.myInd - 1;
  features["PredInd"] = predInd;
  // positions of agents
  for (unsigned int i = 0; i < obs.positions.size(); i++) {
    Point2D diff = getDifferenceToPoint(dims,obs.myPos(),obs.positions[i]);
    std::string key;
    if (i == 0)
      key = "Prey";
    else
      key = "Pred" + boost::lexical_cast<std::string>(i-1);
    features[key + ".dx"] = diff.x;
    features[key + ".dy"] = diff.y;
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
    features[key] = occupied;
  }
  features["NextToPrey"] = next2prey;
  // actions predicted by models
  ActionProbs actionProbs;
  for (boost::unordered_map<std::string,boost::shared_ptr<Agent> >::iterator it = featureAgents.begin(); it != featureAgents.end(); it++) {
    actionProbs = it->second->step(obs);
    features[it->first + ".des"] = actionProbs.maxAction();
  }
}
