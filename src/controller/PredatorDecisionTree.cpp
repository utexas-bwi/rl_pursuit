#include "PredatorDecisionTree.h"
#include <common/WekaParser.h>
#include <factory/AgentFactory.h>

PredatorDecisionTree::PredatorDecisionTree(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &filename):
  Agent(rng,dims),
  filename(filename)
{
  WekaParser parser(filename);
  decisionTree = parser.makeDecisionTree();
  // add the feature agents
  //addFeatureAgent("Greedy.des","greedy");
  //addFeatureAgent("TA.des","ta");
  //addFeatureAgent("GP.des","gp");
  //addFeatureAgent("PD.des","pd");
  addFeatureAgent("Greedy","greedy");
  addFeatureAgent("Team Aware","ta");
  addFeatureAgent("Greedy Prob","gp");
  addFeatureAgent("Prob Dest","pd");
}

ActionProbs PredatorDecisionTree::step(const Observation &obs) {
  Features features;
  Classification action;
  extractFeatures(obs,features);
  decisionTree->classify(features,action);
  return ActionProbs((Action::Type)action);
}

void PredatorDecisionTree::restart() {
}

std::string PredatorDecisionTree::generateDescription() {
  return "PredatorDecisionTree: chooses actions using a decision tree read from: " + filename;
}

void PredatorDecisionTree::extractFeatures(const Observation &obs, Features &features) {
  unsigned int distToPrey = getDistanceToPoint(dims,obs.myPos(),obs.preyPos());
  Point2D diffToPrey = getDifferenceToPoint(dims,obs.myPos(),obs.preyPos());
  features["Next2Prey?"] = (distToPrey == 1);
  features["DeltaXtoP"] = diffToPrey.x;
  features["DeltaYtoP"] = diffToPrey.y;

  ActionProbs actionProbs;
  for (boost::unordered_map<std::string,boost::shared_ptr<Agent> >::iterator it = featureAgents.begin(); it != featureAgents.end(); it++) {
    actionProbs = it->second->step(obs);
    features[it->first] = actionProbs.maxAction();
  }
}

void PredatorDecisionTree::addFeatureAgent(const std::string &key, const std::string &name) {
  featureAgents[key] = createAgent(rng,dims,name,0,Json::Value(),Json::Value()); // the randomNum doesn't matter here
}
