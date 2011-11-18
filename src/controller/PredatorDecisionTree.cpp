#include "PredatorDecisionTree.h"
#include <learning/WekaParser.h>
#include <factory/AgentFactory.h>

PredatorDecisionTree::PredatorDecisionTree(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &filename):
  Agent(rng,dims),
  filename(filename),
  featureExtractor(dims)
{
  WekaParser parser(filename,Action::NUM_ACTIONS,true);
  decisionTree = parser.makeDecisionTree();
  // add the feature agents
  featureExtractor.addFeatureAgent("GR","GR");
  featureExtractor.addFeatureAgent("TA","TA");
  featureExtractor.addFeatureAgent("GP","GP");
  featureExtractor.addFeatureAgent("PD","PD");
}

ActionProbs PredatorDecisionTree::step(const Observation &obs) {
  Features features;
  Classification c;
  featureExtractor.extract(obs,features);
  decisionTree->classify(features,c);
  assert(c.size() == Action::NUM_ACTIONS);
  ActionProbs actionProbs;
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    actionProbs[(Action::Type)i] = c[i];
  return actionProbs;
}

void PredatorDecisionTree::restart() {
}

std::string PredatorDecisionTree::generateDescription() {
  return "PredatorDecisionTree: chooses actions using a decision tree read from: " + filename;
}

