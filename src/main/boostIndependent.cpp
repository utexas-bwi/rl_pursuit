#include <iostream>
#include <vector>
#include <boost/foreach.hpp>

#include <learning/Common.h>
#include <learning/ArffReader.h>
#include <learning/TwoStageTrAdaBoost.h>
#include <factory/ClassifierFactory.h>
#include <factory/AgentFactory.h>
#include <common/Util.h>

std::vector<Feature> getFeatures() {
  std::ifstream in("data/dt/blank.arff");
  ArffReader arff(in);
  in.close();
  return arff.getFeatureTypes();
}

void readAndAddArff(const std::string &filename, Classifier &classifier, bool source, float weight = 1.0, int numSourceSamples = -1) {
  std::cout << "Reading " << filename << " with weight/inst: " << weight << std::endl;
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  int counter = 0;
  while ((!arff.isDone()) && ((numSourceSamples < 0) || (counter < numSourceSamples))) {
    InstancePtr instance = arff.next();
    instance->weight = weight;
    if (source)
      classifier.addSourceData(instance);
    else
      classifier.addData(instance);
    counter++;
  }
  in.close();
}

std::string getArffName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/train/" + student + ".arff";
}

std::string getDTName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/weighted/only-" + student + ".weka";
}
 
int main(int argc, const char *argv[]) {
  if (argc != 5) {
    std::cerr << "Expected 4 arguments" << std::endl;
    std::cerr << "Usage: boostIndependent targetStudent sourceStudent targetDir sourceDir" << std::endl;
    return 1;
  }
  std::string targetStudent = argv[1];
  std::string sourceStudent = argv[2];
  std::string targetDir = argv[3];
  std::string sourceDir = argv[4];

  Json::Value baseLearnerOptions;
  baseLearnerOptions["type"] = "weka";
  baseLearnerOptions["caching"] = false;
  baseLearnerOptions["options"] = "weka.classifiers.trees.REPTree";
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;
  std::vector<Feature> features = getFeatures();

  std::cout << "Creating classifier" << std::endl << std::flush;
  TwoStageTrAdaBoost classifier(features,false,baseLearner,baseLearnerOptions,10,2,-1);
  std::cout << "done creating classifier" << std::endl << std::flush;

  readAndAddArff(getArffName(targetStudent,targetDir),classifier,false);
  readAndAddArff(getArffName(sourceStudent,sourceDir),classifier,true);
  classifier.train();
  std::cout << classifier.getBestSourceInstanceWeight() << " " << sourceStudent << std::endl;
  return 0;
}

