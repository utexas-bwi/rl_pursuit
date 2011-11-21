#ifndef ARFFREADER_996VDU7E
#define ARFFREADER_996VDU7E

/*
File: ArffReader.h
Author: Samuel Barrett
Description: reads in an arff file
Created:  2011-11-18
Modified: 2011-11-18
*/

#include <fstream>
#include <vector>
#include <string>
#include "DecisionTree.h"

class ArffReader {
public:
  ArffReader(const std::string &filename);
  ~ArffReader();
  void next(Features &features); // gets the next set of features
  std::string getClassFeature();
  std::vector<std::string> getFeatureNames();
  bool isDone();

private:
  void readHeader();

private:
  std::ifstream in;
  std::vector<std::string> featureNames;
  std::vector<bool> numeric;
};

#endif /* end of include guard: ARFFREADER_996VDU7E */
