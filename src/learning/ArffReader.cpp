/*
File: ArffReader.cpp
Author: Samuel Barrett
Description: reads in an arff file
Created:  2011-11-18
Modified: 2011-11-21
*/

#include "ArffReader.h"
#include <cassert>
#include <boost/lexical_cast.hpp>

ArffReader::ArffReader(std::ifstream &in):
  in(in)
{
  assert(in.good());
  readHeader();
  assert(in.good());
}
  
ArffReader::~ArffReader() {
}

void ArffReader::next(Instance &features) {
  features.clear();
  float val;
  assert(featureTypes.back().name == WEIGHT_FEATURE); //brittle for now
  for (unsigned int i = 0; i < featureTypes.size()-1; i++) {
    in >> val;
    in.ignore(1,',');
    features[i] = val;
    float weight = 1.0;
    if (in.peek() == '{') {
      // there's a weight for it
      in.ignore(1,'{');
      in >> weight;
      in.ignore(1,'}');
    }
    features[featureTypes.size()-1] = weight;
  }
}

std::string ArffReader::getClassFeature() {
  return featureTypes.back().name;
}

std::vector<Feature> ArffReader::getFeatureTypes() {
  return featureTypes;
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
    Feature feature;
    feature.name = str.substr(startInd+1,endInd-startInd-1);
    feature.numeric = str.substr(endInd + 1) == "numeric";
    if (!feature.numeric) {
      unsigned int start = endInd + 1;
      for (unsigned int i = endInd + 1; i < str.size(); i++) {
        if (str[i] == '{')
          start = i + 1;
        if ((str[i] == ',') || (str[i] == '}')) {
          feature.values.push_back(boost::lexical_cast<int>(str.substr(start,i - start)));
          start = i + 1;
        }
      }
    }
    featureTypes.push_back(feature);
    std::getline(in,str);
  }
  // read until the data
  while (str != "@data")
    std::getline(in,str);
}