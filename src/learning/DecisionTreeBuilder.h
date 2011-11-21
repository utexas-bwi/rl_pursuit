#ifndef DECISIONTREEBUILDER_RALHIB17
#define DECISIONTREEBUILDER_RALHIB17

/*
File: DecisionTreeBuilder.h
Author: Samuel Barrett
Description: trains a decision tree using C4.5
Created:  2011-11-18
Modified: 2011-11-21
*/

#include <vector>
#include <boost/shared_ptr.hpp>
#include <common/DefaultMap.h>
#include "DecisionTree.h"
#include "ArffReader.h"

class DecisionTreeBuilder {
private:
  typedef DefaultMap<unsigned int,unsigned int> ClassCounts;
  struct Split {
    unsigned int featureInd;
    bool numeric;
    float val;
    double gain;
  };
public:
  DecisionTreeBuilder(const std::string &filename, unsigned int numClasses, bool useClassDistributions);
  boost::shared_ptr<DecisionTree> getDecisionTree();

private:
  void buildTree();
  void readInstances(std::vector<Features> &features);

  boost::shared_ptr<DecisionTree::Node> createNode(const std::vector<Features> &instances);
  void getClassCounts(const std::vector<Features> &instances, ClassCounts &classCounts);
  // splits and information gain
  void getBestSplit(const std::vector<Features> &instances, Split &bestSplit);
  double calcIofSet(const std::vector<Features> &instances); // calculates I(P) for a set
  double calcIofP(const std::vector<float> &Pvals);
  bool calcClassFracs(const ClassCounts &classCounts, std::vector<float> &classFracs);
  double calcGainRatio(const std::vector<Features> &instances, const Split &split, double I);

  void splitData(const std::vector<Features> &instances, const Split &split, std::vector<std::vector<Features> > &splitInstances, std::vector<float> &splitVals);
  // make nodes
  boost::shared_ptr<DecisionTree::LeafNode> makeLeaf(const ClassCounts &classCounts);
  boost::shared_ptr<DecisionTree::InteriorNode> makeInterior(const std::vector<Features> &instances, const Split &split);

private:
  static const double MIN_GAIN_RATIO;
  static const unsigned int MIN_INSTANCES_PER_LEAF;

  const std::string &filename;
  unsigned int numClasses;
  bool useClassDistributions;
  std::vector<ArffReader::Feature> featureTypes;
  std::string classFeature;

  boost::shared_ptr<DecisionTree> dt;
};

#endif /* end of include guard: DECISIONTREEBUILDER_RALHIB17 */
