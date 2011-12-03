/*
File: buildDT.cpp
Author: Samuel Barrett
Description: builds a decision tree from the given arff file
Created:  2011-11-21
Modified: 2011-11-21
*/

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>

#include <common/Util.h>
#include <learning/DecisionTree.h>
#include <factory/ClassifierFactory.h>

DEFINE_double(minGain,0.0001,"Minimum gain ratio for splitting nodes of the DT");
DEFINE_int32(minInstances,2,"Minimum number of instances per leaf");
DEFINE_int32(maxDepth,-1,"Max depth of the DT");

int main(int argc, char *argv[]) {
  parseCommandLineArgs(&argc,&argv,"[options] arffFilename",1,1);

  std::string dataFilename = argv[1];
  boost::shared_ptr<DecisionTree> dt = createDecisionTree("",dataFilename,FLAGS_minGain,FLAGS_minInstances,FLAGS_maxDepth);

  std::cout << dt << std::endl;

  return 0;
}
