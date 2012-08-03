#ifndef COMMON_WKEV10M8
#define COMMON_WKEV10M8

/*
File: Common.h
Author: Samuel Barrett
Description: some common defs for learning
Created:  2011-12-01
Modified: 2011-12-01
*/

#include <vector>
#include <map>
#include <set>
#include <string>
#include <stdexcept>
#include <ostream>
#include <boost/shared_ptr.hpp>
#include "Features.h"

typedef std::vector<float> Classification;

struct Instance {
  Instance();
  void clear();
  unsigned int size() const;
  inline float& operator[] (FeatureType_t f) {return data[f];}
  inline float operator[] (FeatureType_t f) const {return data[f];}
  //float& operator[](const std::string &key);
  //float operator[](const std::string &key) const;
  //inline float& operator[](const std::string &key) {
    //return data[key];
  //}
  //inline float operator[](const std::string &key) const {
    //std::map<std::string,float>::const_iterator it = data.find(key);
    //if (it == data.end())
      //throw std::out_of_range("Unknown key: " + key);
    //return it->second;
  //}
  //float get(const std::string& key, float defaultVal) const;
  
  inline float get(FeatureType_t key, float defaultVal) const {
    float val = data[key];
    if (isFeatureUnset(val))
      return defaultVal;
    return val;
  }
  
  //std::map<std::string,float> data;
  float data[FeatureType::NUM];
  unsigned int label;
  float weight;
  bool operator==(const Instance &inst) const;
  friend std::ostream& operator<<(std::ostream &out, const Instance &inst);
};

typedef boost::shared_ptr<Instance> InstancePtr;
typedef boost::shared_ptr<const Instance> InstanceConstPtr;
 
struct FloatCmp {
  static const float EPS;
  bool operator() (const float &x, const float &y) const {
    return x < y - EPS;
  }
};

typedef std::set<float,FloatCmp> FloatSet;

struct InstanceSet {
  InstanceSet(unsigned int numClasses);
  unsigned int getNumClasses() const;
  void add(const InstancePtr &instance);
  void normalize();
  unsigned int size() const;
  InstancePtr operator[](unsigned int ind) const;
  InstancePtr& operator[](unsigned int ind);
  void getValuesForFeature(const FeatureType_t &key, FloatSet &values);
  void recalculateWeight();

  std::vector<InstancePtr> instances;
  float weight;
  Classification classification;
};

typedef boost::shared_ptr<InstanceSet> InstanceSetPtr;


struct Feature {
  //std::string name;
  FeatureType_t feat;
  bool numeric;
  std::vector<unsigned int> values;
  
  friend std::ostream& operator<<(std::ostream &out, const Feature &feat);
};

#endif /* end of include guard: COMMON_WKEV10M8 */
