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
#include <boost/foreach.hpp>
#include <common/Util.h>

#include <learning/WekaParser.h>
#include <learning/ArffReader.h>

#include <learning/AdaBoost.h>
#include <learning/AdaBoostPrime.h>
#include <learning/Committee.h>
#include <learning/DecisionTree.h>
#include <learning/LinearSVM.h>
#include <learning/NaiveBayes.h>
#include <learning/SVM.h>
#include <learning/TrAdaBoost.h>
#include <learning/TrBagg.h>
#include <learning/TwoStageTrAdaBoost.h>
#include <learning/WekaClassifier.h>

ClassifierPtr createClassifier(const Json::Value &options) {
  std::string filename = options.get("filename","").asString();
  return createClassifier(filename,options);
}

ClassifierPtr createClassifier(const std::string &filename, const Json::Value &options) {
  std::string dataFilename = options.get("data","").asString();
  std::vector<Feature> features;
  if (dataFilename != "") {
    std::vector<std::string> dataFilenames;
    boost::split(dataFilenames, dataFilename, boost::is_any_of(";"));
    std::ifstream in(dataFilenames[0].c_str());
    if (!in.good()) {
      std::cerr << "ERROR opening: " << dataFilenames[0] << std::endl;
      std::cerr << "Taken from: " << dataFilename << std::endl;
      exit(17);
    }
    ArffReader arff(in);
    in.close();
    features = arff.getFeatureTypes();
    // remove Trial and Step if they're included, no longer used
    /*
    unsigned int i = 0;
    while (i < features.size()) {
      if ((features[i].name == "Trial") || (features[i].name == "Step")) {
        features.erase(features.begin() + i);
      } else {
        i++;
      }
    }
    */
  } else {
    FeatureType::getFeatures(features);
  }

  return createClassifier(filename,dataFilename,features,options);
}

ClassifierPtr createClassifier(const std::vector<Feature> &features, const Json::Value &options) {
  assert(features.size() > 0);
  return createClassifier("","",features,options);
}

ClassifierPtr createClassifier(const std::string &filename, const std::string &dataFilename, const std::vector<Feature> &features, const Json::Value &options) {
  bool caching = options.get("caching",false).asBool();
  std::string type = options.get("type","dt").asString();
  boost::to_lower(type);
  bool train = options.get("initialTrain",true).asBool();
  bool predictSingleClass = options.get("predictSingleClass",false).asBool();

  ClassifierPtr classifier;
  
  if ((type == "adaboost") || (type == "tradaboost") || (type == "adaboostprime")) {
    classifier = createAdaBoost(type,filename,features,caching,options);
  } else if (type == "dt") {
    classifier = createDecisionTree(filename,features,caching,options);
  } else if ((type == "lsvm") || (type == "linearsvm")) {
    unsigned int maxNumInstances = options.get("maxNumInstances",630000).asUInt();
    unsigned int solverType = options.get("solverType",0).asUInt();
    classifier = ClassifierPtr(new LinearSVM(filename,features,caching,solverType,maxNumInstances));
  } else if (type == "nb") {
    classifier = ClassifierPtr(new NaiveBayes(filename,features,caching));
  } else if (type == "svm") {
    unsigned int maxNumInstances = options.get("maxNumInstances",630000).asUInt();
    classifier = ClassifierPtr(new SVM(features,caching,maxNumInstances));
  } else if (type == "trbagg") {
    classifier = createTrBagg(filename,features,caching,options);
  } else if (type == "twostagetradaboost") {
    classifier = createTwoStageTrAdaBoost(filename,features,caching,options);
  } else if (type == "weka") {
    classifier = createWekaClassifier(filename,features,caching,options);
  } else if (type == "committee") {
    classifier = createCommittee(filename,features,caching,options);
  } else {
    std::cerr << "createClassifier: ERROR, unknown type: " << type << std::endl;
    exit(3);
  }
 
  if (dataFilename != "") {
    std::vector<std::string> dataFilenames;
    boost::split(dataFilenames, dataFilename, boost::is_any_of(";"));

    BOOST_FOREACH(std::string &temp, dataFilenames) {
      std::cout << "ADDING SOURCE DATA: " << temp << std::endl;
      addSourceDataToClassifier(classifier,temp,false);
    }
    if (train)
      classifier->train(false);
      //addSourceDataToClassifier(classifier,dataFilename,train);
  }
  classifier->setPredictSingleClass(predictSingleClass);

  return classifier;
}

void addSourceDataToClassifier(ClassifierPtr classifier, const std::string &dataFilename, bool train) {
  std::cout << "Adding data" << std::endl;
  std::ifstream in(dataFilename.c_str());
  ArffReader arff(in);
  while (!arff.isDone()) {
    InstancePtr instance = arff.next();
    classifier->addSourceData(instance);
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

boost::shared_ptr<AdaBoost> createAdaBoost(const std::string &type, const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  assert(filename == "");
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;
  Json::Value baseLearnerOptions = options["baseLearner"];

  boost::shared_ptr<AdaBoost> c;

  if (type == "adaboost")
    c = boost::shared_ptr<AdaBoost>(new AdaBoost(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations));
  else if (type == "tradaboost")
    c = boost::shared_ptr<AdaBoost>(new TrAdaBoost(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations));
  else if (type == "adaboostprime")
    c = boost::shared_ptr<AdaBoost>(new AdaBoostPrime(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations));
  else {
    std::cerr << "createAdaBoost: ERROR, unknown type: " << type << std::endl;
    exit(3);
  }
  bool verbose = options.get("verbose",true).asBool();
  c->setVerbose(verbose);
  return c;
}

boost::shared_ptr<TwoStageTrAdaBoost> createTwoStageTrAdaBoost(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  assert(filename == "");
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  unsigned int numFolds = options.get("folds",5).asUInt();
  int bestT = options.get("bestT",-1).asInt();
  Json::Value baseLearnerOptions = options["baseLearner"];
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;

  return boost::shared_ptr<TwoStageTrAdaBoost>(new TwoStageTrAdaBoost(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations,numFolds,bestT));
}

boost::shared_ptr<TrBagg> createTrBagg(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options) {
  unsigned int maxBoostingIterations = options.get("maxBoostingIterations",10).asUInt();
  Json::Value baseLearnerOptions = options["baseLearner"];
  Json::Value fallbackLearnerOptions = options["fallbackLearner"];
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;

  boost::shared_ptr<TrBagg> ptr(new TrBagg(features,caching,baseLearner,baseLearnerOptions,maxBoostingIterations,baseLearner,fallbackLearnerOptions));
  if (filename != "")
    assert(ptr->load(filename));
  return ptr;
}

boost::shared_ptr<Committee> createCommittee(const std::string &/*filename*/, const std::vector<Feature> &inFeatures, bool caching, const Json::Value &options) {
  std::vector<Feature> features;
  if (inFeatures.size() == 0) {
    FeatureType::getFeatures(features);
  } else {
    features = inFeatures;
  }
  //std::cout << "CREATING COMMITTEE" << std::endl;
  //for (unsigned int i = 0; i < features.size(); i++)
    //std::cout << "  " << getName(features[i].feat) << std::endl;
  //assert(filename == "");
  Committee::Params p;
  p.fromJson(options);

  // read in sub classifiers
  std::vector<Committee::SubClassifier> classifiers;
  const Json::Value &classifiersJson = options["classifiers"];

  for (unsigned int i = 0; i < classifiersJson.size(); i++) {
    unsigned int numRepeatedModels = classifiersJson[i].get("numRepeatedModels",1).asUInt();
    float weight = classifiersJson[i].get("weight",1.0).asDouble();
    for (unsigned int repeatInd = 0; repeatInd < numRepeatedModels; repeatInd++) {
      // get the options, with $(REPEATED_NUM) filled out
      Json::Value tempOpts(classifiersJson[i]);
      std::map<std::string,std::string> reps;
      reps["$(REPEATED_NUM)"] = boost::lexical_cast<std::string>(repeatInd);
      jsonReplaceStrings(tempOpts,reps);
      //std::cout << "  " << tempOpts.get("filename","").asCString() << std::endl;
      
      // create the classifier
      Committee::SubClassifier c;
      c.weight = weight;
      c.classifier = createClassifier(tempOpts);
      classifiers.push_back(c);
    }
  }

  //std::cout << "HERE" << std::endl << std::flush;
  boost::shared_ptr<Committee> x(new Committee(features,caching,classifiers,p));
  //std::cout << "HERE2" << std::endl << std::flush;
  return x;
}
