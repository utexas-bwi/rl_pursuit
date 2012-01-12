/*
File: Classifier.cpp
Author: Samuel Barrett
Description: abstract classifier
Created:  2011-12-27
Modified: 2011-12-27
*/

#include "Classifier.h"
#include <iostream>

std::size_t hash_value(const Instance &i) {
  std::size_t seed = 0;
  boost::hash_combine(seed,i.data.size());
  for (std::map<std::string,float>::const_iterator it = i.data.begin(); it != i.data.end(); it++)
    boost::hash_combine(seed,it->second);
  return seed;
}

Classifier::Classifier(const std::vector<Feature> &features, bool caching):
  features(features),
  numClasses(features.back().values.size()),
  caching(caching)
{
  if (caching) {
    std::cout << "CACHING" << std::endl;
    cache = boost::shared_ptr<ClassificationCache>(new ClassificationCache());
  }
}

std::ostream& Classifier::outputHeader(std::ostream &out) const {
  out << "@relation 'Classifier as represented by learning/Classifier.h'" << std::endl;
  out << std::endl;
  for (unsigned int i = 0; i < features.size(); i++)
    out << "@attribute " << features[i] << std::endl;
  out << std::endl;
  out << "@data";
  return out;
}

void Classifier::train(bool incremental)  {
  if (caching)
    cache->clear();
  //std::cout << "TRAINING " << std::boolalpha << incremental << std::endl;
  trainInternal(incremental);
}

void Classifier::classify(const InstancePtr &instance, Classification &classification) {
  if (caching) {
    ClassificationCache::iterator it = cache->find(*instance);
    if (it != cache->end()) {
      classification = it->second;
      return;
    } 
  }
  classification.resize(numClasses,0);
  classifyInternal(instance,classification);
  if (caching)
    (*cache)[*instance] = classification;
}
  
std::ostream& operator<<(std::ostream &out, const Classifier &c) {
  c.outputDescription(out);
  return out;
}
