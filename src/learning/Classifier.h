#ifndef CLASSIFIER_4OFDPALS
#define CLASSIFIER_4OFDPALS

/*
File: Classifier.h
Author: Samuel Barrett
Description: abstract classifier
Created:  2011-11-22
Modified: 2011-11-22
*/

#include <string>
#include <vector>
#include <map>
#include <stdexcept>

typedef std::vector<float> Classification;
//typedef std::vector<float> Instance;
//typedef std::map<std::string,float> Instance;
const std::string WEIGHT_FEATURE = "__WEIGHT_FEATURE__";

struct Instance {
  std::map<std::string,float> data;

  void clear() {
    data.clear();
  }

  unsigned int size() {
    return data.size();
  }

  float& weight() {
    return data[WEIGHT_FEATURE];
  }

  float weight() const {
    return data.find(WEIGHT_FEATURE)->second;
  }

  float& operator[](const std::string &key) {
    return data[key];
  }
  
  float operator[](const std::string &key) const{
    std::map<std::string,float>::const_iterator it = data.find(key);
    if (it == data.end())
      throw std::out_of_range("Unknown key: " + key);
    return it->second;
  }

  friend std::ostream& operator<<(std::ostream &out, const Instance &inst) {
    out << "<Instance ";
    for (std::map<std::string,float>::const_iterator it = inst.data.begin(); it != inst.data.end(); it++)
      out << it->first << ":" << it->second << ",";
    out << ">";
    return out;
  }
};

struct Feature {
  std::string name;
  bool numeric;
  std::vector<unsigned int> values;
  
  friend std::ostream& operator<<(std::ostream &out, const Feature &feat) {
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
};

class Classifier {
public:
  Classifier(const std::vector<Feature> &features, const std::string &classFeature):
    features(features),
    classFeature(classFeature)
  {
    for (unsigned int i = 0; i < features.size(); i++) {
      if (features[i].name == classFeature) {
        numClasses = features[i].values.size();
        break;
      }
    }
  }

  virtual ~Classifier() {}
  virtual void addData(const Instance &instance) = 0;
  virtual void train() = 0;
  virtual void classify(const Instance &instance, Classification &classification) = 0;

  std::ostream& outputHeader(std::ostream &out) const {
    out << "@relation 'Classifier as represented by learning/Classifier.h'" << std::endl;
    out << std::endl;
    for (unsigned int i = 0; i < features.size(); i++)
      out << "@attribute " << features[i] << std::endl;
    out << std::endl;
    out << "@data" << std::endl;
    return out;
  }

protected:
  std::vector<Feature> features;
  const std::string classFeature;
  unsigned int numClasses;
};

#endif /* end of include guard: CLASSIFIER_4OFDPALS */
