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
    // other naming scheme
    std::string key;
    if (it->first == "Greedy")
      key = "Greedy";
    else if (it->first == "Team Aware")
      key = "TA";
    else if (it->first == "Greedy Prob")
      key = "GP";
    else if (it->first == "Prob Dest")
      key = "PD";
    key += ".des";
    features[key] = actionProbs.maxAction();
  }

  // naming scheme 2
  features["Next2Prey"] = (distToPrey == 1);
  features["DeltaXToPrey"] = diffToPrey.x;
  features["DeltaYToPrey"] = diffToPrey.y;
  for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
    std::string key = "Occupied";
    switch (i) {
      case Action::UP:
        key += "U";
        break;
      case Action::DOWN:
        key += "D";
        break;
      case Action::LEFT:
        key += "L";
        break;
      case Action::RIGHT:
        key += "R";
        break;
      default:
        assert(false);
    }
    bool occupied = false;
    for (unsigned int j = 0; j < obs.positions.size(); j++) {
      if (movePosition(dims,obs.myPos(),(Action::Type)i) == obs.positions[j]) {
        occupied = true;
        break;
      }
    }
    features[key] = occupied;
  }

  features["Prey.x"] = obs.preyPos().x;
  features["Prey.y"] = obs.preyPos().y;

  assert(obs.preyInd == 0);
  for (unsigned int i = 1; i < obs.positions.size(); i++) {
    std::string key = "Pred" + boost::lexical_cast<std::string>(i-1);
    features[key + ".x"] = obs.positions[i].x;
    features[key + ".y"] = obs.positions[i].y;
  }
}

void PredatorDecisionTree::addFeatureAgent(const std::string &key, const std::string &name) {
  featureAgents[key] = createAgent(rng,dims,name,0,0,Json::Value(),Json::Value()); // the trialNum and predatorInd don't matter here
}
