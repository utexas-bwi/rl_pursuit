/*
File: Classifier.cpp
Author: Samuel Barrett
Description: abstract classifier
Created:  2011-12-27
Modified: 2011-12-27
*/

#include "Classifier.h"
#include <iostream>
#include <rl_pursuit/common/Util.h>
#include <boost/lexical_cast.hpp>

std::size_t hash_value(const Instance &inst) {
  std::size_t seed = 0;
  for (unsigned int i = 0; i < FeatureType::NUM; i++)
    boost::hash_combine(seed,inst.data[i]);
  return seed;
}
//std::size_t hash_value(const Instance &i) {
  //std::size_t seed = 0;
  //boost::hash_combine(seed,i.data.size());
  //for (std::map<std::string,float>::const_iterator it = i.data.begin(); it != i.data.end(); it++)
    //boost::hash_combine(seed,it->second);
  //return seed;
//}

Classifier::Classifier(const std::vector<Feature> &features, bool caching):
  features(features),
  numClasses(features.back().values.size()),
  caching(caching),
  predictSingleClass(false),
  rng(new RNG(0))
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
  classification.resize(numClasses);
  for (unsigned int i = 0; i < classification.size(); i++)
    classification[i] = 0.0;
  classifyInternal(instance,classification);
  if (predictSingleClass) {
    unsigned int maxInd = vectorMaxInd(classification);
    for (unsigned int i = 0; i < classification.size(); i++)
      classification[i] = 0;
    classification[maxInd] = 1.0;
  }
  if (caching)
    (*cache)[*instance] = classification;
}

void Classifier::setPredictSingleClass(bool flag) {
  if (flag)
    std::cout << "SETTING PREDICT SINGLE CLASS" << std::endl;
  predictSingleClass = flag;
}


std::string Classifier::getSubFilename(const std::string &baseFilename, const std::string &sub) const {
  size_t ind = baseFilename.rfind(".");
  assert(ind != std::string::npos);
  return baseFilename.substr(0,ind) + "-" + sub + baseFilename.substr(ind);
}

std::string Classifier::getSubFilename(const std::string &baseFilename, unsigned int i) const {
  return getSubFilename(baseFilename,boost::lexical_cast<std::string>(i));
}
  
std::vector<std::string> Classifier::getSubFilenames(const std::string &baseFilename, unsigned int maxInd) const {
  std::vector<std::string> filenames(maxInd);
  for (unsigned int i = 0; i < maxInd; i++)
    filenames[i] = getSubFilename(baseFilename,i);
  return filenames;
}
  
std::ostream& operator<<(std::ostream &out, const Classifier &c) {
  c.outputDescription(out);
  return out;
}
