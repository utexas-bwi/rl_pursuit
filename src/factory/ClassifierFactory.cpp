/*
File: ClassifierFactory.cpp
Author: Samuel Barrett
Description: generates classifiers
Created:  2011-12-02
Modified: 2011-12-28
*/

#include "ClassifierFactory.h"
#include <iostream>
#include <cassert>
#include <boost/algorithm/string.hpp>

#include <learning/WekaParser.h>
#include <learning/ArffReader.h>
#include <learning/DecisionTree.h>
#include <learning/WekaClassifier.h>
#include <learning/TrAdaBoost.h>

ClassifierPtr createClassifier(const std::string &filename, const Json::Value &options) {
  std::string type = options.get("type","dt").asString();
  boost::to_lower(type);
  std::string dataFilename = options.get("data","").asString();
  bool caching = options.get("caching",false).asBool();
  ClassifierPtr classifier;
  
  std::vector<Feature> features;
  if (dataFilename != "") {
    std::ifstream in(dataFilename.c_str());
    ArffReader arff(in);
    in.close();
    features = arff.getFeatureTypes();
  }

  if (type == "dt") {
    classifier = createDecisionTree(filename,features,caching,options);
  } else if (type == "weka") {
    classifier = createWekaClassifier(filename,features,caching,options);
  } else if (type == "tradaboost") {
    classifier = createTrAdaBoost(filename,dataFilename,caching,options);
  } else {
    std::cerr << "createClassifier: ERROR, unknown type: " << type << std::endl;
    exit(3);
  }

  if (dataFilename != "")
    addDataToClassifier(classifier,dataFilename);

  return classifier;
}

void addDataToClassifier(ClassifierPtr classifier, const std::string &dataFilename) {
  std::cout << "Adding data" << std::endl;
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    classifier->addData(instance);
  }
  in.close();
  classifier->train(false);
}

boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  double minGainRatio = options.get("minGain",0.0001).asDouble();
  unsigned int minInstancesPerLeaf = options.get("minInstances",2).asUInt();
  int maxDepth = options.get("maxDepth",-1).asInt();
  return createDecisionTree(filename,features,caching,minGainRatio,minInstancesPerLeaf,maxDepth);
}

boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::vector<Feature> &features, bool caching, double minGainRatio, unsigned int minInstancesPerLeaf, int maxDepth) {
  boost::shared_ptr<DecisionTree> dt;
  if (filename != "") {
    dt = createDecisionTreeFromWeka(filename,caching);
  } else {
    assert(features.size() > 0);
    dt = boost::shared_ptr<DecisionTree>(new DecisionTree(features,caching));
  }
  dt->setLearningParams(minGainRatio, minInstancesPerLeaf, maxDepth);
  return dt;
}

boost::shared_ptr<DecisionTree> createDecisionTreeFromWeka(const std::string &filename, bool caching) {
  WekaParser parser(filename,Action::NUM_ACTIONS);
  return parser.makeDecisionTree(caching);
}

boost::shared_ptr<WekaClassifier> createWekaClassifier(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  assert(filename == "");
  assert(features.size() > 0);
  std::string wekaOptions = options.get("options","").asString();
  return boost::shared_ptr<WekaClassifier>(new WekaClassifier(features,caching, wekaOptions));
}

boost::shared_ptr<DecisionTree> createBoostDT(const std::vector<Feature> &features, bool caching) {
  boost::shared_ptr<DecisionTree> dt(new DecisionTree(features,caching));
  dt->setLearningParams(0.0001,2,1); // stumpy
  return dt;
}

boost::shared_ptr<TrAdaBoost> createTrAdaBoost(const std::string &filename, std::string &dataFilename, bool caching, const Json::Value &options) {
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  assert(filename == "");
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  boost::shared_ptr<TrAdaBoost> c(new TrAdaBoost(arff.getFeatureTypes(),caching,&createBoostDT,maxBoostingIterations));
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    c->addSourceData(instance);
  }
  in.close();
  c->train(false);
  dataFilename = ""; // so that create classifier doesn't do any additional training
  return c;
}
