#ifndef CLASSIFIER_4OFDPALS
#define CLASSIFIER_4OFDPALS

/*
File: Classifier.h
Author: Samuel Barrett
Description: abstract classifier
Created:  2011-11-22
Modified: 2011-11-22
*/

#include "Common.h"

class Classifier {
public:
  Classifier(const std::vector<Feature> &features):
    features(features),
    numClasses(features.back().values.size())
  {
  }

  virtual ~Classifier() {}
  virtual void addData(const InstancePtr &instance) = 0;
  virtual void train() = 0;
  virtual void classify(const InstancePtr &instance, Classification &classification) const = 0;

  std::ostream& outputHeader(std::ostream &out) const {
    out << "@relation 'Classifier as represented by learning/Classifier.h'" << std::endl;
    out << std::endl;
    for (unsigned int i = 0; i < features.size(); i++)
      out << "@attribute " << features[i] << std::endl;
    out << std::endl;
    out << "@data";
    return out;
  }

protected:
  std::vector<Feature> features;
  const std::string classFeature;
  unsigned int numClasses;
};

#endif /* end of include guard: CLASSIFIER_4OFDPALS */
