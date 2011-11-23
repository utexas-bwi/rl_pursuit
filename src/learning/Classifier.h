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

typedef std::vector<float> Classification;
typedef std::vector<float> Instance;

struct Feature {
  std::string name;
  bool numeric;
  std::vector<unsigned int> values;
};

class Classifier {
public:
  Classifier(const std::vector<Feature> &features, unsigned int classInd, unsigned int weightInd):
    features(features),
    classInd(classInd),
    weightInd(weightInd),
    numClasses(features[classInd].values.size())
  {
  }

  virtual ~Classifier();
  virtual void addData(const Instance &instance) = 0;
  virtual void train() = 0;
  virtual void classify(const Instance &instance, Classification &classification) = 0;

protected:
  std::vector<Feature> features;
  unsigned int classInd;
  unsigned int weightInd;
  unsigned int numClasses;
};

#endif /* end of include guard: CLASSIFIER_4OFDPALS */
