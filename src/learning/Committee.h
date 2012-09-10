#ifndef COMMITTEE_KHNUEZIS
#define COMMITTEE_KHNUEZIS

/*
File: Committee.h
Author: Samuel Barrett
Description: committee of classifiers
Created:  2012-08-09
Modified: 2012-08-09
*/

#include "Classifier.h"
#include "SubClassifier.h"
#include <common/Params.h>

class Committee: public Classifier {
public:
#define PARAMS(_)
  // no specific params yet
  Params_STRUCT(PARAMS)
#undef PARAMS

public:
  Committee(const std::vector<Feature> &features, bool caching, const std::vector<SubClassifier> &classifiers, const Params &p);
  virtual void addData(const InstancePtr &instance);
  virtual void outputDescription(std::ostream &out) const;
  
  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);

  void normalizeWeights();

protected:
  std::vector<SubClassifier> classifiers;
  Params p;
};

#endif /* end of include guard: COMMITTEE_KHNUEZIS */
