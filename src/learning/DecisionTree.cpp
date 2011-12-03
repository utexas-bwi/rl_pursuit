/*
File: DecisionTree.cpp
Author: Samuel Barrett
Description: a decision tree
Created:  2011-12-01
Modified: 2011-12-01
*/

#include "DecisionTree.h"
#include <cmath>
#include <iostream>
#include <limits>

#undef DEBUG_DT_SPLITS

//const double DecisionTree::MIN_GAIN_RATIO = 0.0001;
//const unsigned int DecisionTree::MIN_INSTANCES_PER_LEAF = 2;
const float DecisionTree::EPS = 0.0001;

////////////////////
// INTERIOR NODE
////////////////////

DecisionTree::InteriorNode::InteriorNode(ComparisonOperator cmp, const std::string &splitKey):
  cmp(cmp),
  splitKey(splitKey)
{
}

void DecisionTree::InteriorNode::addChild(const NodePtr &child, float splitValue) {
  children.push_back(child);
  splitValues.push_back(splitValue);
}

void DecisionTree::InteriorNode::classify(const InstancePtr &instance, Classification &classification) const {
  getChild(instance)->classify(instance,classification);
}

void DecisionTree::InteriorNode::addData(const InstancePtr &instance) {
  getChild(instance)->addData(instance);
}

DecisionTree::NodePtr DecisionTree::InteriorNode::getChild(const InstancePtr &instance) const {
  float const &val = (*instance)[splitKey];
  for (unsigned int i = 0; i < splitValues.size(); i++) {
    switch (cmp) {
      case EQUALS:
        if (fabs(val - splitValues[i]) < EPS)
          return children[i];
        break;
      case LESS:
        if (val < splitValues[i])
          return children[i];
        break;
      case LEQ:
        if (val <= splitValues[i])
          return children[i];
        break;
      default:
        std::cerr << "DecisionTree::InteriorNode::getChild: ERROR, bad comparison: " << cmp << std::endl;
        exit(5);
        break;
    }
  }
  // was last child then
  return children.back();
}

void DecisionTree::InteriorNode::train(NodePtr &, const DecisionTree &dt, int maxDepth) {
  if (maxDepth == 0)
    return;
  for (unsigned int i = 0; i < children.size(); i++) {
    children[i]->train(children[i],dt, maxDepth - 1);
  }
}

void DecisionTree::InteriorNode::output(std::ostream &out, unsigned int depth) {
  out << std::endl;
  for (unsigned int i = 0; i < splitValues.size(); i++){
    for (unsigned int j = 0; j < depth; j++)
      out << "|  ";
    out << splitKey << " ";
    ComparisonOperator c = cmp;
    if ((c == LESS) && (i == splitValues.size() - 1))
      c = GEQ;
    if ((c == LEQ) && (i == splitValues.size() - 1))
      c = GREATER;
    switch (c) {
      case EQUALS:
        out << "=";
        break;
      case LESS:
        out << "<";
        break;
      case GEQ:
        out << ">=";
        break;
      case LEQ:
        out << "<=";
        break;
      case GREATER:
        out << ">";
        break;
    }
    out << " " << splitValues[i];
    children[i]->output(out,depth+1);
  }
}


////////////////////
// LEAF NODE
////////////////////

DecisionTree::LeafNode::LeafNode(const InstanceSetPtr &instances):
  instances(instances)
{
}

void DecisionTree::LeafNode::classify(const InstancePtr &, Classification &classification) const {
  classification = instances->classification;
}

void DecisionTree::LeafNode::addData(const InstancePtr &instance) {
  instances->add(instance);
}

void DecisionTree::LeafNode::train(NodePtr &ptr, const DecisionTree &dt, int maxDepth) {
  if (maxDepth == 0)
    return;
  if (instances->weight <= EPS) {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No data, skipping" << std::endl;
#endif
    return;
  }
  // normalize the classes
  //std::cout << "*" << std::endl;
  instances->normalize();
  //std::cout << "*" << std::endl;
  // only split if we have multiple clases
  if (oneClass()) {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No split necessary, one class already" << std::endl;
#endif
  } else {
    //std::cout << "*** trySplittingNode with weight " << instances->weight << std::endl;
    trySplittingNode(ptr,dt,maxDepth);
    //std::cout << "*** done splittingNode" << std::endl;
  }
}

void DecisionTree::LeafNode::trySplittingNode(NodePtr &ptr, const DecisionTree &dt, int maxDepth) {
  double I = dt.calcIofSet(instances);
  Split bestSplit;
  bestSplit.gain = -1 * std::numeric_limits<float>::infinity();

  for (unsigned int i = 0; i < dt.features.size(); i++) {
    Feature const &feature = dt.features[i];
    if (feature.numeric) {
      FloatSet vals;
      instances->getValuesForFeature(feature.name,vals);

      FloatSet::iterator it = vals.begin();
      FloatSet::iterator it2 = vals.begin();
      if (it != vals.end())
        it++; // drop the first values
      for (; it != vals.end(); it++, it2++) {
        //std::cout << "it: " << *it << std::endl;
        Split split;
        split.featureInd = i;
        split.val = (*it + *it2) * 0.5;
        dt.calcGainRatio(instances,split,I);
        if (split.gain > bestSplit.gain)
          bestSplit = split;
      }
    } else {
      Split split;
      split.featureInd = i;
      dt.calcGainRatio(instances,split,I);
      if (split.gain > bestSplit.gain)
        bestSplit = split;
    }
  }

  if (bestSplit.gain > dt.MIN_GAIN_RATIO) {
    Feature const &feature = dt.features[bestSplit.featureInd];
#ifdef DEBUG_DT_SPLITS
    std::cout << "best split = " << feature.name;
    if (feature.numeric)
      std::cout << " < " << bestSplit.val;
    std::cout << std::endl;
    std::cout << "best split gain = " << bestSplit.gain << std::endl;
#endif
    ComparisonOperator op = EQUALS;
    if (feature.numeric)
      op = LESS;
    boost::shared_ptr<InteriorNode> interior(new InteriorNode(op,feature.name));
    for (unsigned int i = 0; i < bestSplit.instanceSets.size(); i++) {
      NodePtr leaf(new LeafNode(bestSplit.instanceSets[i]));
      interior->addChild(leaf,bestSplit.splitVals[i]);
    }
    // change the pointer to point to the new interior node
    ptr = interior;
    // keep training
    interior->train(ptr,dt,maxDepth);
  } else {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No useful splits found" << std::endl;
#endif
  }
}

bool DecisionTree::LeafNode::oneClass() const {
  unsigned int counter = 0;
  Classification &c = instances->classification;
  for (unsigned int i = 0; i < c.size(); i++) {
    if (c[i] > EPS) {
      counter++;
      if (counter >= 2)
        return false;
    }
  }
  //std::cout << "Counter " << counter << " total weight: " << instances->weight << std::endl;
  return true;
}

void DecisionTree::LeafNode::output(std::ostream &out, unsigned int) {
  out << ":";
  for (unsigned int i = 0; i < instances->classification.size(); i++)
    out << " " << instances->classification[i];
  out << std::endl;
}

////////////////////
// MAIN FUNCTIONS
////////////////////
DecisionTree::DecisionTree(const std::vector<Feature> &features, NodePtr root):
  Classifier(features),
  root(root)
{
  if (this->root.get() == NULL) {
    InstanceSetPtr instances(new InstanceSet(numClasses));
    this->root = NodePtr(new DecisionTree::LeafNode(instances));
  }
  setLearningParams();
}

void DecisionTree::setLearningParams(double minGainRatio, unsigned int minInstancesPerLeaf, int maxDepth) {
  MIN_GAIN_RATIO = minGainRatio;
  MIN_INSTANCES_PER_LEAF = minInstancesPerLeaf;
  MAX_DEPTH = maxDepth;
}

void DecisionTree::addData(const InstancePtr &instance) {
  root->addData(instance);
}

void DecisionTree::classify(const InstancePtr &instance, Classification &classification) const {
  root->classify(instance,classification);
}

void DecisionTree::train() {
  root->train(root,*this,MAX_DEPTH);
}

void DecisionTree::calcGainRatio(const InstanceSetPtr &instances, DecisionTree::Split &split, double I) const {
  splitData(instances,split);
  unsigned int numAcceptable = 0;
  for (unsigned int i = 0; i < split.instanceSets.size(); i++)
    if (split.instanceSets[i]->size() >= MIN_INSTANCES_PER_LEAF)
      numAcceptable++;
  if (numAcceptable < 2) {
    split.gain = -1 * std::numeric_limits<double>::infinity();
    return;
  }

  double info = 0;
  std::vector<float> ratios(split.instanceSets.size());
  for (unsigned int i = 0; i < split.instanceSets.size(); i++) {
    split.instanceSets[i]->normalize();
    ratios[i] = split.instanceSets[i]->weight / instances->weight;
    //std::cout << "ratios[" << i << "]: " << ratios[i] << " " << split.instanceSets[i]->weight << " " << instances->weight  << std::endl;
    info += ratios[i] * calcIofSet(split.instanceSets[i]); 
  }
  double gain = I - info;
  double splitInfo = calcIofP(ratios);
  double gainRatio = gain / splitInfo;
  split.gain = gainRatio;
}

double DecisionTree::calcIofSet(const InstanceSetPtr &instances) const { 
  return calcIofP(instances->classification);
}

double DecisionTree::calcIofP(const Classification &Pvals) const {
  double I = 0; 
  //std::cout << "PVALS: ";
  //for (unsigned int j = 0; j < Pvals.size(); j++)
    //std::cout << Pvals[j] << " ";
  //std::cout << std::endl;
  //assert((Pvals.size() == 5) || (Pvals.size() == 2) || (Pvals.size() == 4));
  for (unsigned int i = 0; i < Pvals.size(); i++) {
    //assert(Pvals[i] <= 1.0); // TODO REMOVE THIS
    if (Pvals[i] > 0) // don't take the log of 0 :)
      I -= Pvals[i] * log(Pvals[i]);
  }
  return I;
}

void DecisionTree::splitData(const InstanceSetPtr &instances, Split &split) const {
  Feature const &feature = features[split.featureInd];

  //std::cout << "SPLITTING DATA on: " << feature.name << std::endl;
  if (feature.numeric) {
    split.splitVals.push_back(split.val);
    split.splitVals.push_back(std::numeric_limits<float>::infinity());
  } else {
    for (unsigned int i = 0; i < feature.values.size(); i++)
      split.splitVals.push_back(feature.values[i]);
  }
  split.instanceSets.resize(split.splitVals.size());
  for (unsigned int i = 0; i < split.splitVals.size(); i++)
    split.instanceSets[i] = InstanceSetPtr(new InstanceSet(instances->getNumClasses()));

  for (unsigned int i = 0; i < instances->size(); i++) {
    float val = (*(*instances)[i])[feature.name];
    if (!feature.numeric)
      val -= 0.5;
    for (unsigned int j = 0; j < split.splitVals.size(); j++) {
      if (val < split.splitVals[j]) {
        split.instanceSets[j]->add((*instances)[i]);
        break;
      }
    }
  }
  if (feature.numeric)
    split.splitVals[1] = split.splitVals[0];
}

std::ostream& operator<<(std::ostream &out, const DecisionTree &dt) {
  dt.outputHeader(out);
  dt.root->output(out,0);
  return out;
}
