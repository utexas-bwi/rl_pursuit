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
#include <boost/algorithm/string.hpp>

ArffReader::ArffReader(std::ifstream &in):
  in(in)
{
  assert(in.good());
  readHeader();
  //assert(!in.fail());
  //assert(in.good());
}
  
ArffReader::~ArffReader() {
}

InstancePtr ArffReader::next() {
  float val;
  InstancePtr instance(new Instance());
  instance->weight = 1.0;
  for (unsigned int i = 0; i < featureTypes.size(); i++) {
    in >> val;
    in.ignore(1,',');
    (*instance)[featureTypes[i].feat] = val;
  }
  instance->label = (*instance)[getClassFeature()];
  // check if there's a weight
  if (in.peek() == '{') {
    in.ignore(1,'{');
    in >> instance->weight;
    in.ignore(1,'}');
  }
  return instance;
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
  while (in.good()) {
    std::getline(in,str);
    if (str.compare(0,start.size(),start) != 0) {
      header += str + '\n';
      continue;
    }
    break;
  }
  // read in the attributes
  while (in.good()) {
    if (str.compare(0,start.size(),start) != 0)
      break;
    header += str + '\n';
    startInd = str.find(" ",start.size()-1);
    endInd = str.find(" ",startInd+1);
    Feature feature;
    std::string name = str.substr(startInd+1,endInd-startInd-1);
    boost::replace_all(name,".","_"); // because . is unusable for the enum
    feature.feat = FeatureType::fromName(name);
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
  while (in.good() && (str != "@data")) {
    std::getline(in,str);
    header += str + '\n';
  }
  // check if there's an additional blank lines
  while (true) {
    std::streampos prevPos = in.tellg();
    std::getline(in,str);
    if (str != "") {
      in.seekg(prevPos);
      break;
    }
    if (in.eof())
      break;
  }
}
