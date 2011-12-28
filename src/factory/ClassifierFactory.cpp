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

boost::shared_ptr<Classifier> createClassifier(const std::string &filename, const Json::Value &options) {
  std::string type = options.get("type","dt").asString();
  boost::to_lower(type);
  std::string dataFilename = options.get("data","").asString();
  bool caching = options.get("caching",false).asBool();

  if (type == "dt") {
    return createDecisionTree(filename,dataFilename,caching,options);
  } else if (type == "weka") {
    return createWekaClassifier(filename,dataFilename,caching,options);
  } else {
    std::cerr << "createClassifier: ERROR, unknown type: " << type << std::endl;
    exit(3);
  }
}

boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::string &dataFilename, bool caching, const Json::Value &options) {
  double minGainRatio = options.get("minGain",0.0001).asDouble();
  unsigned int minInstancesPerLeaf = options.get("minInstances",2).asUInt();
  int maxDepth = options.get("maxDepth",-1).asInt();
  return createDecisionTree(filename,dataFilename,caching,minGainRatio,minInstancesPerLeaf,maxDepth);
}

boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::string &dataFilename, bool caching, double minGainRatio, unsigned int minInstancesPerLeaf, int maxDepth) {
  boost::shared_ptr<DecisionTree> dt;
  if (filename != "") {
    dt = createDecisionTreeFromWeka(filename,caching);
  } else {
    assert(dataFilename != "");
    dt = createBlankDecisionTreeFromArff(dataFilename,caching);
  }
  dt->setLearningParams(minGainRatio, minInstancesPerLeaf, maxDepth);
  if (dataFilename != "") {
    addDataToDecisionTree(dt,dataFilename);
  }
  return dt;
}

boost::shared_ptr<DecisionTree> createDecisionTreeFromWeka(const std::string &filename, bool caching) {
  WekaParser parser(filename,Action::NUM_ACTIONS);
  return parser.makeDecisionTree(caching);
}

boost::shared_ptr<DecisionTree> createBlankDecisionTreeFromArff(const std::string &dataFilename, bool caching) {
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  in.close();
  boost::shared_ptr<DecisionTree> dt(new DecisionTree(arff.getFeatureTypes(),caching));
  return dt;
}

void addDataToDecisionTree(boost::shared_ptr<DecisionTree> dt, const std::string &dataFilename) {
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    dt->addData(instance);
  }
  in.close();
  dt->train();
}

boost::shared_ptr<WekaClassifier> createWekaClassifier(const std::string &/*filename*/, const std::string &dataFilename, bool caching, const Json::Value &options) {
  assert(dataFilename != "");
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  in.close();
  std::string wekaOptions = options.get("options","").asString();
  return boost::shared_ptr<WekaClassifier>(new WekaClassifier(arff.getFeatureTypes(),caching, wekaOptions));
}
