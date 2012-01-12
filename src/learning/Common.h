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

typedef std::vector<float> Classification;

struct Instance {
  Instance();
  void clear();
  unsigned int size() const;
  float& operator[](const std::string &key);
  float operator[](const std::string &key) const;
  float get(const std::string& key, float defaultVal) const;
  
  std::map<std::string,float> data;
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
  void getValuesForFeature(const std::string &key, FloatSet &values);
  void recalculateWeight();

  std::vector<InstancePtr> instances;
  float weight;
  Classification classification;
};

typedef boost::shared_ptr<InstanceSet> InstanceSetPtr;


struct Feature {
  std::string name;
  bool numeric;
  std::vector<unsigned int> values;
  
  friend std::ostream& operator<<(std::ostream &out, const Feature &feat);
};

#endif /* end of include guard: COMMON_WKEV10M8 */
