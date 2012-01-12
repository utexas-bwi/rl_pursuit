/*
File: Common.cpp
Author: Samuel Barrett
Description: som common defs for learning
Created:  2011-12-01
Modified: 2011-12-01
*/

#include "Common.h"
#include <cmath>

Instance::Instance():
  label(0),
  weight(0)
{
}

void Instance::clear() {
  data.clear();
  label = 0;
  weight = 0;
}

unsigned int Instance::size() const {
  return data.size();
}

float& Instance::operator[](const std::string &key) {
  return data[key];
}

float Instance::operator[](const std::string &key) const{
  std::map<std::string,float>::const_iterator it = data.find(key);
  if (it == data.end())
    throw std::out_of_range("Unknown key: " + key);
  return it->second;
}

float Instance::get(const std::string& key, float defaultVal) const {
  std::map<std::string,float>::const_iterator it = data.find(key);
  if (it == data.end())
    return defaultVal;
  return it->second;
}

bool Instance::operator==(const Instance &inst) const {
  const float EPS = 0.001f;
  if (size() != inst.size()) {
    return false;
  }
  for (std::map<std::string,float>::const_iterator it = data.begin(); it != data.end(); it++) {
    if (fabs(it->second - inst[it->first]) > EPS)
      return false;
  }
  return true;
}

std::ostream& operator<<(std::ostream &out, const Instance &inst) {
  out << "<Instance ";
  for (std::map<std::string,float>::const_iterator it = inst.data.begin(); it != inst.data.end(); it++)
    out << it->first << ":" << it->second << ",";
  out << "WEIGHT:" << inst.weight;
  out << ">";
  return out;
}

const float FloatCmp::EPS = 0.0001;

InstanceSet::InstanceSet(unsigned int numClasses):
  weight(0),
  classification(numClasses, 1.0 / numClasses)
{
}

unsigned int InstanceSet::getNumClasses() const {
  return classification.size();
}

void InstanceSet::add(const InstancePtr &instance) {
  instances.push_back(instance);
  for (unsigned int i = 0; i < classification.size(); i++)
    classification[i] *= weight / (weight + instance->weight);
  classification[instance->label] += instance->weight / (weight + instance->weight);
  weight += instance->weight;
}

void InstanceSet::normalize() {
  if (weight <= 0) {
    //std::cout << "randomizing for weight: " << weight << std::endl;
    for (unsigned int i = 0; i < classification.size(); i++)
      classification[i] = 1.0 / classification.size();
  } else {
    //std::cout << "normalizing with weight: " << weight << std::endl;
    for (unsigned int i = 0; i < classification.size(); i++)
      classification[i] = 0;
    for (unsigned int i = 0; i < instances.size(); i++)
      classification[instances[i]->label] += instances[i]->weight / weight;
    //float total = 0;
    //for (unsigned int i = 0; i < classification.size(); i++)
      //total += classification[i];
    //for (unsigned int i = 0; i < classification.size(); i++)
      //classification[i] /= total;
  }
}

unsigned int InstanceSet::size() const {
  return instances.size();
}

InstancePtr InstanceSet::operator[](unsigned int ind) const{
  return instances[ind];
}

InstancePtr& InstanceSet::operator[](unsigned int ind) {
  return instances[ind];
}

void InstanceSet::getValuesForFeature(const std::string &key, FloatSet &values) {
  for (unsigned int i = 0; i < instances.size(); i++) {
    values.insert((*instances[i])[key]);
  }
}

void InstanceSet::recalculateWeight() {
  weight = 0;
  for (unsigned int i = 0; i < instances.size(); i++)
    weight += instances[i]->weight;
}

std::ostream& operator<<(std::ostream &out, const Feature &feat) {
  out << feat.name << " ";
  if (feat.numeric)
    out << "numeric";
  else {
    out << "{";
    for (unsigned int i = 0; i < feat.values.size(); i++) {
      if (i != 0)
        out << ",";
      out << feat.values[i];
    }
    out << "}";
  }
  return out;
}
