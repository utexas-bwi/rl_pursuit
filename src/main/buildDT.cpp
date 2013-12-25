/*
File: buildDT.cpp
Author: Samuel Barrett
Description: builds a decision tree from the given arff file
Created:  2011-11-21
Modified: 2011-11-21
*/

#include <iostream>
#include <fstream>
#include <rl_pursuit/gflags/gflags.h>

#include <rl_pursuit/common/Util.h>
#include <rl_pursuit/learning/DecisionTree.h>
#include <rl_pursuit/factory/ClassifierFactory.h>

DEFINE_double(minGain,0.0001,"Minimum gain ratio for splitting nodes of the DT");
DEFINE_int32(minInstances,2,"Minimum number of instances per leaf");
DEFINE_int32(maxDepth,-1,"Max depth of the DT");

int main(int argc, char *argv[]) {
  parseCommandLineArgs(&argc,&argv,"[options] arffFilename",1,1);

  std::string dataFilename = argv[1];
  Json::Value options;
  options["type"] = "dt";
  options["initialTrain"] = true;
  options["data"] = argv[1];
  options["maxDepth"] = FLAGS_maxDepth;
  options["minGain"] = FLAGS_minGain;
  options["minInstances"] = FLAGS_minInstances;

  std::cerr << "Training a DT with minGain:" << FLAGS_minGain << " minInstances:" << FLAGS_minInstances << " maxDepth:" << FLAGS_maxDepth << std::endl;
  //boost::shared_ptr<DecisionTree> dt = createDecisionTree("",dataFilename,FLAGS_minGain,FLAGS_minInstances,FLAGS_maxDepth);
  ClassifierPtr c = createClassifier(options);

  std::cout << *c << std::endl;

  return 0;
}
