/*
File: PredatorClassifier.cpp
Author: Samuel Barrett
Description: a predator that selects actions using a decision tree
Created:  2011-09-15
Modified: 2011-12-02
*/

#include "PredatorClassifier.h"
#include <factory/AgentFactory.h>
#include <boost/lexical_cast.hpp>

PredatorClassifier::PredatorClassifier(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<Classifier> classifier, const std::string &name, int trainingPeriod, bool trainIncremental):
  Agent(rng,dims),
  name(name),
  classifier(classifier),
  featureExtractor(dims),
  trainingPeriod(trainingPeriod),
  trainingCounter(0),
  trainIncremental(trainIncremental)
{
  // add the feature agents
  //featureExtractor.addFeatureAgent("GR","GR");
  //featureExtractor.addFeatureAgent("TA","TA");
  //featureExtractor.addFeatureAgent("GP","GP");
  //featureExtractor.addFeatureAgent("PD","PD");
}

ActionProbs PredatorClassifier::step(const Observation &obs) {
  Classification c;
  InstancePtr instance = featureExtractor.extract(obs,stepHistory);
  classifier->classify(instance,c);
  assert(c.size() == Action::NUM_ACTIONS);
  ActionProbs actionProbs;
  for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
    actionProbs[(Action::Type)i] = c[i];
  return actionProbs;
}

void PredatorClassifier::restart() {
  trainingCounter = 0;
  stepHistory.reset();
}

std::string PredatorClassifier::generateDescription() {
  std::string msg = "PredatorClassifier: chooses actions using a classifier";
  if (trainingPeriod >= 0)
    msg += " with training period " + boost::lexical_cast<std::string>(trainingPeriod);
  else
    msg += " with no online training";
  std::stringstream ss;
  classifier->outputDescription(ss);
  msg += "\n" + ss.str();
  return msg;
}

void PredatorClassifier::learn(const Observation &prevObs, const Observation &currentObs, unsigned int ind) {
  if (trainingPeriod < 0)
    return;

  Point2D move = getDifferenceToPoint(dims,prevObs.positions[ind],currentObs.positions[ind]);
  InstancePtr instance = featureExtractor.extract(prevObs,learnHistory);
  instance->label = getAction(move);
  (*instance)["Pred.act"] = instance->label;
  classifier->addData(instance);
  trainingCounter++;
  if (trainingCounter >= trainingPeriod) {
    //std::cout << "training" << std::endl;
    trainingCounter = 0;
    classifier->train(trainIncremental);
  }
    //std::cout << "*************************************" << std::endl;
    //DecisionTree *dt = (DecisionTree*)(classifier.get());
    //std::cout << *dt << std::endl;
    //std::cout << "*************************************" << std::endl;
}
  
void PredatorClassifier::minimalStep(const Observation &obs) {
  featureExtractor.updateHistory(obs,stepHistory);
}
