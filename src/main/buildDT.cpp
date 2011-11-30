/*
File: buildDT.cpp
Author: Samuel Barrett
Description: builds a decision tree from the given arff file
Created:  2011-11-21
Modified: 2011-11-21
*/

#include <iostream>
#include <fstream>
#include <learning/ArffReader.h>
#include <learning/DecisionTree.h>

int main(int argc, const char *argv[]) {
  char usage[] = "Usage: buildDT arffFilename";
  if (argc != 2) {
    std::cerr << usage << std::endl;
    return 1;
  }
  
  std::ifstream in(argv[1]);
  ArffReader arff(in);
  Instance instance;
  DecisionTree dt(arff.getFeatureTypes(),arff.getClassFeature());
  while (!arff.isDone()) {
    arff.next(instance);
    dt.addData(instance);
  }
  dt.train();
  in.close();
  //DecisionTreeBuilder builder(filename,5,true);
  //boost::shared_ptr<DecisionTree> dt = builder.getDecisionTree();
  //std::cout << dt->root;

  return 0;
}
