#ifndef DECISIONTREE_AX8LS8WW
#define DECISIONTREE_AX8LS8WW

/*
File: DecisionTree.h
Author: Samuel Barrett
Description: a decision tree
Created:  2011-09-13
Modified: 2011-09-13
*/

#include <vector>
#include <set>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <limits>
#include "Classifier.h"


class DecisionTree: public Classifier {
public:
////////////////////
// HELPER CLASSES
////////////////////
  enum ComparisonOperator{
    EQUALS,
    LESS,
    GEQ,
    LEQ,
    GREATER
  };
  
  struct Split {
    std::string feature;
    unsigned int featureInd;
    float val;
    double gain;
  };

  struct Node {
    virtual void classify(const Instance &instance, Classification &classification, bool adjustClassificationQ=false, unsigned int trueClass=0, float weight=1.0) = 0;
    virtual std::ostream& genDescription(std::ostream &out, unsigned int depth = 0) = 0;
    virtual bool isLeaf()=0;
    virtual void randomizeUnseenLeaves() = 0;
    virtual void generalizeUnseenLeaves(Classification &general) = 0;
    virtual void setGeneralization(const Classification &general) = 0;
  };
  struct InteriorNode: public Node {
    InteriorNode(ComparisonOperator cmp, const std::string &splitKey);
    void addChild(boost::shared_ptr<Node> child, float splitValue);
    void classify(const Instance &instance, Classification &classification, bool adjustClassificationQ=false, unsigned int trueClass=0, float weight=1.0);
    unsigned int getInd(const Instance &instance);
    std::ostream& genDescription(std::ostream &out, unsigned int depth = 0);
    bool isLeaf(){return false;}
    void randomizeUnseenLeaves();
    void generalizeUnseenLeaves(Classification &general);
    void setGeneralization(const Classification &general);

    const ComparisonOperator cmp;
    const std::string splitKey;
    std::vector<boost::shared_ptr<Node> > children;
    std::vector<float> splitValues;
    static const float EPS;
  };

  struct LeafNode: public Node {
    LeafNode(const Classification &classification, const std::vector<Instance> &instances = std::vector<Instance>());
    void classify(const Instance &instance, Classification &classification, bool adjustClassificationQ=false, unsigned int trueClass=0, float weight=1.0);
    std::ostream& genDescription(std::ostream &out, unsigned int depth = 0);
    void adjustClassification(unsigned int trueClass, float weight);
    bool isLeaf(){return true;}
    void randomizeUnseenLeaves();
    void generalizeUnseenLeaves(Classification &general);
    void setGeneralization(const Classification &general);
    bool isUnseen();

    Classification classification;
    float total;
    std::vector<Instance> instances;
  };

////////////////////
// MAIN FUNCTIONS
////////////////////
  DecisionTree(const std::vector<Feature> &features, const std::string &classFeature, boost::shared_ptr<Node> root = boost::shared_ptr<Node>());
  virtual ~DecisionTree();

  void addData(const Instance &instance);
  void train();
  void classify(const Instance &instance, Classification &classification);

  void classify(const Instance &instance, Classification &classification, bool adjustClassificationQ, unsigned int trueClass, float weight);
  void randomizeUnseenLeaves();
  void generalizeUnseenLeaves();

////////////////////
// TRAIN FUNCTIONS
////////////////////
  boost::shared_ptr<Node> createNode(const std::vector<Instance> &instances);
  bool getClassFracs(const std::vector<Instance> &instances, Classification &classFracs);
  // splits and information gain
  void getBestSplit(const std::vector<Instance> &instances, Split &bestSplit);
  double calcIofSet(const std::vector<Instance> &instances); // calculates I(P) for a set
  double calcIofP(const Classification &Pvals);
  double calcGainRatio(const std::vector<Instance> &instances, const Split &split, double I);
  void splitData(const std::vector<Instance> &instances, const Split &split, std::vector<std::vector<Instance> > &splitInstances, std::vector<float> &splitVals);
  // make nodes
  boost::shared_ptr<LeafNode> makeLeaf(const Classification &classification, const std::vector<Instance> &instances);
  boost::shared_ptr<InteriorNode> makeInterior(const std::vector<Instance> &instances, const Split &split);

private:
  boost::shared_ptr<Node> root;
  std::vector<Instance> instances;
  bool useClassDistributions;
  
  static const double MIN_GAIN_RATIO;
  static const unsigned int MIN_INSTANCES_PER_LEAF;

  friend std::ostream& operator<<(std::ostream &out, const DecisionTree &dt);
};

std::ostream& operator<<(std::ostream &out, boost::shared_ptr<DecisionTree::Node> node);
std::ostream& operator<<(std::ostream &out, const DecisionTree &dt);

#endif /* end of include guard: DECISIONTREE_AX8LS8WW */
