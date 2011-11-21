/*
File: ArffReader.cpp
Author: Samuel Barrett
Description: reads in an arff file
Created:  2011-11-18
Modified: 2011-11-18
*/

#include "ArffReader.h"
#include <cassert>

ArffReader::ArffReader(const std::string &filename):
  in(filename.c_str())
{
  assert(in.good());
  readHeader();
  assert(in.good());
}
  
ArffReader::~ArffReader() {
  in.close();
}
  
void ArffReader::next(Features &features) {
  features.clear();
  float val;
  for (unsigned int i = 0; i < featureNames.size(); i++) {
    in >> val;
    in.ignore(1,',');
    features[featureNames[i]] = val;
    float weight = 1.0;
    if (in.peek() == '{') {
      // there's a weight for it
      in.ignore(1,'{');
      in >> weight;
      in.ignore(1,'}');
    }
    features[WEIGHT_FEATURE] = weight;
  }
}

std::string ArffReader::getClassFeature() {
  return featureNames.back();
}

std::vector<std::string> ArffReader::getFeatureNames() {
  return featureNames;
}

bool ArffReader::isDone() {
  return in.eof();
}

void ArffReader::readHeader() {
  std::string str;
  std::string start = "@attribute ";
  int startInd;
  int endInd;

  // read until the attributes
  while (true) {
    std::getline(in,str);
    if (str.compare(0,start.size(),start) != 0)
      continue;
    break;
  }
  // read in the attributes
  while (true) {
    if (str.compare(0,start.size(),start) != 0)
      break;

    startInd = str.find(" ",start.size()-1);
    endInd = str.find(" ",startInd+1);
    featureNames.push_back(str.substr(startInd+1,endInd-startInd-1));
    numeric.push_back(str.substr(endInd+1) == "numeric");
    std::getline(in,str);
  }
  // read until the data
  while (str != "@data")
    std::getline(in,str);
}
