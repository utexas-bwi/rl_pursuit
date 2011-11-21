/*
File: buildDT.cpp
Author: Samuel Barrett
Description: builds a decision tree from the given arff file
Created:  2011-11-21
Modified: 2011-11-21
*/

#include <iostream>
#include <learning/DecisionTreeBuilder.h>

int main(int argc, const char *argv[]) {
  char usage[] = "Usage: buildDT arffFilename";
  if (argc != 2) {
    std::cerr << usage << std::endl;
    return 1;
  }
  
  std::string filename(argv[1]);
  DecisionTreeBuilder builder(filename,5,true);
  boost::shared_ptr<DecisionTree> dt = builder.getDecisionTree();
  std::cout << dt->root << std::endl;

  return 0;
}
