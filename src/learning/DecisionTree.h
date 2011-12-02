#ifndef DECISIONTREE_EOHEH8YA
#define DECISIONTREE_EOHEH8YA

/*
File: DecisionTree.h
Author: Samuel Barrett
Description: a decision tree
Created:  2011-12-01
Modified: 2011-12-01
*/

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
    unsigned int featureInd;
    double gain;
    float val;
    std::vector<InstanceSetPtr> instanceSets;
    std::vector<float> splitVals;
  };

///////////////////
// NODES
///////////////////
  class Node;
  typedef boost::shared_ptr<Node> NodePtr;

  class Node {
  public:
    virtual void classify(const InstancePtr &instance, Classification &classification) const = 0;
    virtual void addData(const InstancePtr &instance) = 0;
    virtual void train(NodePtr &ptr, const DecisionTree &dt) = 0;
    virtual void output(std::ostream &out, unsigned int depth) = 0;
  };

  class InteriorNode: public Node {
  public:
    InteriorNode(ComparisonOperator cmp, const std::string &splitKey);
    void addChild(const NodePtr &child, float splitValue);
    void classify(const InstancePtr &instance, Classification &classification) const;
    void addData(const InstancePtr &instance);
    void train(NodePtr &ptr, const DecisionTree &dt);
    void output(std::ostream &out, unsigned int depth = 0);
  private:
    NodePtr getChild(const InstancePtr &instance) const;
  private:
    const ComparisonOperator cmp;
    const std::string splitKey;
    std::vector<NodePtr> children;
    std::vector<float> splitValues;
  };

  class LeafNode: public Node {
  public:
    LeafNode(const InstanceSetPtr &instances);
    void classify(const InstancePtr &instance, Classification &classification) const;
    void addData(const InstancePtr &instance);
    void train(NodePtr &ptr, const DecisionTree &dt);
    void output(std::ostream &out, unsigned int depth = 0);
  private:
    bool oneClass() const;
    void trySplittingNode(NodePtr &ptr, const DecisionTree &dt);
  private:
    InstanceSetPtr instances;
  };
  
////////////////////
// MAIN FUNCTIONS
////////////////////
public:
  DecisionTree(const std::vector<Feature> &features, NodePtr root = NodePtr(), double minGainRatio = 0.0001, unsigned int minInstancesPerLeaf = 2);
  
  void addData(const InstancePtr &instance);
  void classify(const InstancePtr &instance, Classification &classification) const;
  void train();

private:
  void calcGainRatio(const InstanceSetPtr &instances, Split &split, double I) const;
  double calcIofSet(const InstanceSetPtr &instances) const; // calculates I(P) for a set
  double calcIofP(const Classification &Pvals) const;
  void splitData(const InstanceSetPtr &instances, Split &split) const;
private:
  NodePtr root;
  const double MIN_GAIN_RATIO;
  const unsigned int MIN_INSTANCES_PER_LEAF;
  static const float EPS;

  friend class Node;
  friend std::ostream& operator<<(std::ostream &out, const DecisionTree &dt);
};


#endif /* end of include guard: DECISIONTREE_EOHEH8YA */
