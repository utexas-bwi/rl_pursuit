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

#include <learning/AdaBoost.h>
#include <learning/DecisionTree.h>
#include <learning/NaiveBayes.h>
#include <learning/TrAdaBoost.h>
#include <learning/WekaClassifier.h>

ClassifierPtr createClassifier(const Json::Value &options) {
  std::string filename = options.get("filename","").asString();
  return createClassifier(filename,options);
}

ClassifierPtr createClassifier(const std::string &filename, const Json::Value &options) {
  std::string dataFilename = options.get("data","").asString();
  std::vector<Feature> features;
  if (dataFilename != "") {
    std::ifstream in(dataFilename.c_str());
    ArffReader arff(in);
    in.close();
    features = arff.getFeatureTypes();
  }
  return createClassifier(filename,dataFilename,features,options);
}

ClassifierPtr createClassifier(const std::vector<Feature> &features, const Json::Value &options) {
  return createClassifier("","",features,options);
}

ClassifierPtr createClassifier(const std::string &filename, const std::string &inDataFilename, const std::vector<Feature> &features, const Json::Value &options) {
  std::string dataFilename(inDataFilename); // workaround for TrAdaBoost
  bool caching = options.get("caching",false).asBool();
  std::string type = options.get("type","dt").asString();
  boost::to_lower(type);
  bool train = options.get("initialTrain",true).asBool();

  ClassifierPtr classifier;
  
  if (type == "adaboost") {
    classifier = createAdaBoost(filename,features,caching,options);
  } else if (type == "dt") {
    classifier = createDecisionTree(filename,features,caching,options);
  } else if (type == "nb") {
    classifier = ClassifierPtr(new NaiveBayes(features,caching));
  } else if (type == "tradaboost") {
    classifier = createTrAdaBoost(filename,dataFilename,caching,options,train);
  } else if (type == "weka") {
    classifier = createWekaClassifier(filename,features,caching,options);
  } else {
    std::cerr << "createClassifier: ERROR, unknown type: " << type << std::endl;
    exit(3);
  }
  
  if (dataFilename != "")
    addDataToClassifier(classifier,dataFilename,train);

  return classifier;
}

void addDataToClassifier(ClassifierPtr classifier, const std::string &dataFilename, bool train) {
  std::cout << "Adding data" << std::endl;
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    classifier->addData(instance);
  }
  in.close();
  if (train)
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
  dt->setLearningParams(0.0001,2,2); // stumpy
  return dt;
}

boost::shared_ptr<WekaClassifier> createBoostWeka(const std::vector<Feature> &features, bool caching) {
  std::string wekaOptions = "weka.classifiers.trees.J48";
  return boost::shared_ptr<WekaClassifier>(new WekaClassifier(features,caching, wekaOptions));
}

boost::shared_ptr<AdaBoost> createAdaBoost(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  assert(filename == "");
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;
  Json::Value baseLearnerOptions = options["baseLearner"];

  return boost::shared_ptr<AdaBoost>(new AdaBoost(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations));
}

boost::shared_ptr<TrAdaBoost> createTrAdaBoost(const std::string &filename, std::string &dataFilename, bool caching, const Json::Value &options, bool train) {
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  assert(filename == "");
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  Json::Value baseLearnerOptions = options["baseLearner"];
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;
  boost::shared_ptr<TrAdaBoost> c(new TrAdaBoost(arff.getFeatureTypes(),caching,baseLearner,baseLearnerOptions,maxBoostingIterations));
  //boost::shared_ptr<TrAdaBoost> c(new TrAdaBoost(arff.getFeatureTypes(),caching,&createBoostWeka,maxBoostingIterations));
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    c->addSourceData(instance);
  }
  in.close();
  dataFilename = ""; // so that create classifier doesn't do any additional training
  if (train)
    c->train(false);
  return c;
}
