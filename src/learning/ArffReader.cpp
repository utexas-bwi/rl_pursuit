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

InstancePtr ArffReader::next() {
  float val;
  InstancePtr instance(new Instance());
  instance->weight = 1.0;
  for (unsigned int i = 0; i < featureTypes.size() - 1; i++) {
    in >> val;
    in.ignore(1,',');
    (*instance)[featureTypes[i].name] = val;
  }
  // TODO assuming class is last feature
  in >> instance->label;
  in.ignore(1,',');
  // check if there's a weight
  if (in.peek() == '{') {
    in.ignore(1,'{');
    in >> instance->weight;
    in.ignore(1,'}');
  }
  return instance;
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

std::string ArffReader::getHeader() {
  return header;
}

void ArffReader::readHeader() {
  std::string str;
  std::string start = "@attribute ";
  int startInd;
  int endInd;

  // read until the attributes
  while (true) {
    std::getline(in,str);
    if (str.compare(0,start.size(),start) != 0) {
      header += str + '\n';
      continue;
    }
    break;
  }
  // read in the attributes
  while (true) {
    if (str.compare(0,start.size(),start) != 0)
      break;
    header += str + '\n';
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
  header += str + '\n';
  // read until the data
  while (str != "@data") {
    std::getline(in,str);
    header += str + '\n';
  }
}
