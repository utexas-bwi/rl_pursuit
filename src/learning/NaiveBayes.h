#ifndef NAIVEBAYES_MYB94WB7
#define NAIVEBAYES_MYB94WB7

/*
File: NaiveBayes.h
Author: Samuel Barrett
Description: naive bayes classifier
Created:  2012-01-12
Modified: 2012-01-12
*/

#include "Classifier.h"


class NaiveBayes: public Classifier {
public:
  struct Attribute {
    bool numeric;
    // discrete
    std::vector<std::vector<float> > probs; // vals by classes
    // continuous
    std::vector<float> means;
    std::vector<float> stdevs;
  };

  NaiveBayes(const std::vector<Feature> &features, bool caching);
  virtual ~NaiveBayes();
  virtual void addData(const InstancePtr &instance);

  virtual void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);

  void learnDiscreteAttribute(const Feature &feature);
  void learnContinuousAttribute(const Feature &feature);
  void predictDiscreteAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c);
  void predictContinuousAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c);

protected:
  InstanceSet data;
  std::vector<Attribute> attributes;
  static const float ALPHA;
};

#endif /* end of include guard: NAIVEBAYES_MYB94WB7 */
