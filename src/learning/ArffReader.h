#ifndef ARFFREADER_996VDU7E
#define ARFFREADER_996VDU7E

/*
File: ArffReader.h
Author: Samuel Barrett
Description: reads in an arff file
Created:  2011-11-18
Modified: 2011-11-21
*/

#include <fstream>
#include <vector>
#include <string>
#include "DecisionTree.h"

class ArffReader {
public:
  struct Feature {
    std::string name;
    bool numeric;
    std::vector<unsigned int> values;
  };

  ArffReader(const std::string &filename);
  ~ArffReader();
  void next(Features &features); // gets the next set of features
  std::string getClassFeature();
  std::vector<Feature> getFeatureTypes();
  bool isDone();

private:
  void readHeader();

private:
  std::ifstream in;
  std::vector<Feature> featureTypes;
};

#endif /* end of include guard: ARFFREADER_996VDU7E */
