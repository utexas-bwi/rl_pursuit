#include "PredatorDecisionTree.h"
#include <factory/AgentFactory.h>

PredatorDecisionTree::PredatorDecisionTree(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<Classifier> classifier, const std::string &name):
  Agent(rng,dims),
  name(name),
  classifier(classifier),
  featureExtractor(dims)
{
  // add the feature agents
  featureExtractor.addFeatureAgent("GR","GR");
  featureExtractor.addFeatureAgent("TA","TA");
  featureExtractor.addFeatureAgent("GP","GP");
  featureExtractor.addFeatureAgent("PD","PD");
}

ActionProbs PredatorDecisionTree::step(const Observation &obs) {
  Classification c;
  InstancePtr instance = featureExtractor.extract(obs);
  classifier->classify(instance,c);
  assert(c.size() == Action::NUM_ACTIONS);
  ActionProbs actionProbs;
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    actionProbs[(Action::Type)i] = c[i];
  return actionProbs;
}

void PredatorDecisionTree::restart() {
}

std::string PredatorDecisionTree::generateDescription() {
  return "PredatorDecisionTree: chooses actions using a decision tree read from: " + name;
}

