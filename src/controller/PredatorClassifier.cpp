/*
File: PredatorClassifier.cpp
Author: Samuel Barrett
Description: a predator that selects actions using a decision tree
Created:  2011-09-15
Modified: 2011-12-02
*/

#include "PredatorClassifier.h"
#include <factory/AgentFactory.h>

PredatorClassifier::PredatorClassifier(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<Classifier> classifier, const std::string &name):
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

ActionProbs PredatorClassifier::step(const Observation &obs) {
  Classification c;
  InstancePtr instance = featureExtractor.extract(obs);
  classifier->classify(instance,c);
  assert(c.size() == Action::NUM_ACTIONS);
  ActionProbs actionProbs;
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    actionProbs[(Action::Type)i] = c[i];
  return actionProbs;
}

void PredatorClassifier::restart() {
}

std::string PredatorClassifier::generateDescription() {
  return "PredatorClassifier: chooses actions using a decision tree read from: " + name;
}

