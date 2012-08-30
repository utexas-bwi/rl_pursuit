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
  ArffReader(std::ifstream &in);
  ~ArffReader();
  InstancePtr next(); // gets the next set of features
  inline FeatureType_t getClassFeature() {
    // TODO assuming class is last feature
    return featureTypes.back().feat;
  }
  std::vector<Feature> getFeatureTypes();
  bool isDone();
  std::string getHeader();

private:
  void readHeader();

private:
  std::ifstream &in;
  std::vector<Feature> featureTypes;
  std::string header;
  std::vector<bool> useFeatures;
};

#endif /* end of include guard: ARFFREADER_996VDU7E */
