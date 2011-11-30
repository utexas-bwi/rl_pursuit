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
    return data.find(key)->second;
  }
};

struct Feature {
  std::string name;
  bool numeric;
  std::vector<unsigned int> values;
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

protected:
  std::vector<Feature> features;
  const std::string classFeature;
  unsigned int numClasses;
};

#endif /* end of include guard: CLASSIFIER_4OFDPALS */
