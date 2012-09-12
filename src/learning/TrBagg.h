#ifndef TRBAGG_LVCVABXW
#define TRBAGG_LVCVABXW

/*
File: TrBagg.h
Author: Samuel Barrett
Description: implementation of the TrBagg algorithm
Created:  2012-01-18
Modified: 2012-01-18
*/

#include "Classifier.h"
#include "SubClassifier.h"
#include <common/RNG.h>

class TrBagg: public Classifier {
public:
  TrBagg(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations, SubClassifierGenerator fallbackLearner, const Json::Value &fallbackLearnerOptions);
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);

  virtual void outputDescription(std::ostream &out) const;
  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);
  bool partialLoad(const std::string &filename);
  virtual void clearData();

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification, const std::vector<SubClassifier> &classifiers);

  virtual void calcErrorOfClassifier(SubClassifier &c);
  virtual double calcErrorOfSet(unsigned int size);
  virtual double calcErrorOfSet(const std::vector<SubClassifier> &classifiers);

  virtual unsigned int selectSize(const std::vector<SubClassifier> &classifiers);
  virtual double calcErrorOfSet(unsigned int size, const std::vector<std::vector<Classification> > &classifications);
  void insertClassifier(const SubClassifier &c);

protected:
  SubClassifierGenerator baseLearner;
  Json::Value baseLearnerOptions;
  SubClassifierGenerator fallbackLearner;
  Json::Value fallbackLearnerOptions;
  std::vector<SubClassifier> classifiers;
  InstanceSet data;
  const unsigned int maxBoostingIterations;
  int targetDataStart;
  bool didPartialLoad;
  std::vector<SubClassifier> partiallyLoadedClassifiers;
};

#endif /* end of include guard: TRBAGG_LVCVABXW */
