/*
File: DecisionTree.cpp
Author: Samuel Barrett
Description: a decision tree
Created:  2011-09-13
Modified: 2011-09-13
*/

#include "DecisionTree.h"
#include <cmath>
#include <cassert>
#include <fstream>
#include <iostream>

//////////////////////////////////////////////////////////////
// INTERIOR NODE
//////////////////////////////////////////////////////////////

const float DecisionTree::InteriorNode::EPS = 0.0001;
  
DecisionTree::InteriorNode::InteriorNode(ComparisonOperator cmp, unsigned int splitInd):
  cmp(cmp),
  splitInd(splitInd)
{
}

void DecisionTree::InteriorNode::addChild(boost::shared_ptr<Node> child, float splitValue) {
  children.push_back(child);
  splitValues.push_back(splitValue);
}

void DecisionTree::InteriorNode::classify(const Instance &instance, Classification &classification, bool adjustClassificationQ, unsigned int trueClass, float weight) {
  try {
    //std::cout << "SPLIT on " << splitKey << std::endl;
    unsigned int ind = getInd(instance);
    children[ind]->classify(instance,classification,adjustClassificationQ,trueClass,weight);
  } catch(std::out_of_range) {
    std::cerr << "DecisionTree::InteriorNode::classify failed for feature: " << splitInd << std::endl;
    exit(5);
  }
}
unsigned int DecisionTree::InteriorNode::getInd(const Instance &instance) {
  float val = instance[splitInd];
  for (unsigned int i = 0; i < splitValues.size(); i++) {
    switch(cmp) {
      case EQUALS:
        if (fabs(val-splitValues[i]) < EPS)
          return i;
        break;
      case LESS:
        if (val < splitValues[i])
          return i;
        break;
      case LEQ:
        if (val <= splitValues[i])
          return i;
        break;
      default:
        std::cerr << "DecisionTree::InteriorNode::classify: ERROR, bad comparison: " << cmp << std::endl;
        exit(5);
        break;
    }
  }
  // was last ind then
  return splitValues.size()-1;
}

std::ostream& DecisionTree::InteriorNode::genDescription(std::ostream &out, unsigned int depth) {

  for (unsigned int i = 0; i < splitValues.size(); i++){
    for (unsigned int j = 0; j < depth; j++)
      out << "|  ";
    out << splitInd << " ";
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
    if (!children[i]->isLeaf())
      out << std::endl;
    children[i]->genDescription(out,depth+1);
    if (children[i]->isLeaf())
      out << std::endl;
  }
  //out << "(Interior " << splitKey << " " << cmp << std::endl;
  //out << "Children: " << std::endl;
  //for (unsigned int i = 0; i < splitValues.size(); i++) {
    //out << "test : " << splitValues[i] << std::endl;
    //out << "child: " << children[i];
  //}
  //out << ")" << std::endl;
  return out;
}

void DecisionTree::InteriorNode::randomizeUnseenLeaves() {
  for (unsigned int i = 0; i < children.size(); i++)
    children[i]->randomizeUnseenLeaves();
}

void DecisionTree::InteriorNode::generalizeUnseenLeaves(Classification &general) {
  //general.clear();
  Classification general2;
  for (unsigned int i = 0; i < children.size(); i++)
    children[i]->generalizeUnseenLeaves(general2);
  if (general2.empty())
    general2 = general;
  else {
    if (general.empty())
      general = general2;
    else {
      for (unsigned int i = 0; i < children.size(); i++)
        general[i] += general2[i];
    }
  }

  for (unsigned int i = 0; i < children.size(); i++)
    children[i]->setGeneralization(general2);
}

void DecisionTree::InteriorNode::setGeneralization(const Classification &general) {
  for (unsigned int i = 0; i < children.size(); i++)
    children[i]->setGeneralization(general);
}

//////////////////////////////////////////////////////////////
// LEAF NODE
//////////////////////////////////////////////////////////////

DecisionTree::LeafNode::LeafNode(const Classification &classification):
  classification(classification),
  total(0)
{
}

void DecisionTree::LeafNode::classify(const Instance &, Classification &c, bool adjustClassificationQ, unsigned int trueClass, float weight) {
  if (adjustClassificationQ)
    adjustClassification(trueClass,weight);
  c = classification;
}

void DecisionTree::LeafNode::adjustClassification(unsigned int trueClass,float weight) {
  assert(trueClass < classification.size());
  float frac = total / (total + weight);
  for (unsigned int i = 0; i < classification.size(); i++) {
    if (i == trueClass)
      classification[i] = (classification[i] * total + weight) / (total + weight);
    else
      classification[i] *= frac;
  }
  total += weight;
}


std::ostream& DecisionTree::LeafNode::genDescription(std::ostream &out, unsigned int ) {
  //for (unsigned int i = 0; i < depth; i++)
    //out << "  ";
  //out << "(Leaf";
  //for (unsigned int i = 0; i < classification.size(); i++)
    //out << " " << (int)(classification[i] * total+0.5);
  //out << " - " << total;
  //out << ")" << std::endl;
  out << ":";
  for (unsigned int i = 0; i < classification.size(); i++)
    out << " " << classification[i];
  return out;
}

void DecisionTree::LeafNode::randomizeUnseenLeaves() {
  if (!isUnseen()) // if not unseen, don't randomize
    return;
  float frac = 1.0 / classification.size();
  for (unsigned int i = 0; i < classification.size(); i++)
    classification[i] = frac;
}

void DecisionTree::LeafNode::generalizeUnseenLeaves(Classification &general) {
  //std::cout << total << std::endl;
  if (isUnseen()) // if unseen, have no data to add
    return;
  while (general.size() < classification.size())
    general.push_back(0);
  assert(general.size() == classification.size());
  for (unsigned int i = 0; i < general.size(); i++)
    general[i] += classification[i] * total;
}

void DecisionTree::LeafNode::setGeneralization(const Classification &general) {
  if (!isUnseen()) // if not unseen, don't need the generalization
    return;
  if (fabs(total + 1) < 0.0001)
    return; //already generalized
  float generalTotal = 0;
  for (unsigned int i = 0; i < general.size(); i++)
    generalTotal += general[i];
  for (unsigned int i = 0; i < general.size(); i++)
    classification[i] = general[i] / generalTotal;
  total = -1;
}

bool DecisionTree::LeafNode::isUnseen() {
  return total < 0.0001;
}


//////////////////////////////////////////////////////////////
// DECISION TREE
//////////////////////////////////////////////////////////////

DecisionTree::DecisionTree(const std::vector<Feature> &features, unsigned int classInd, unsigned int weightInd):
  Classifier(features,classInd,weightInd)
{
}
DecisionTree::DecisionTree(const std::vector<Feature> &features, boost::shared_ptr<Node> root, unsigned int classInd, unsigned int weightInd):
  Classifier(features,classInd,weightInd),
  root(root)
{
}

DecisionTree::~DecisionTree() {
}

void DecisionTree::addData(const Instance &instance) {
  instances.push_back(instance);
}
  
void DecisionTree::train() {
  root = createNode(instances);
}

void DecisionTree::classify(const Instance &instance, Classification &classification) {
  classify(instance,classification,false,0,1.0);
}

void DecisionTree::classify(const Instance &instance, Classification &classification, bool adjustClassificationQ, unsigned int trueClass, float weight) {
  root->classify(instance,classification,adjustClassificationQ,trueClass,weight);
}
  
void DecisionTree::randomizeUnseenLeaves() {
  root->randomizeUnseenLeaves();
}

void DecisionTree::generalizeUnseenLeaves() {
  Classification general;
  root->generalizeUnseenLeaves(general);
}

////////////////////
// TRAIN FUNCTIONS
////////////////////
boost::shared_ptr<DecisionTree::Node> DecisionTree::createNode(const std::vector<Instance> &instances) {
  Classification classFracs(numClasses);
  bool hasData = getClassFracs(instances,classFracs);
  if (!hasData) {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No data, randomizing" << std::endl;
#endif
    for (unsigned int i = 0; i < numClasses; i++)
      classFracs[i] = 1.0 / numClasses;
    return makeLeaf(classFracs);
  }

  // check if it only has one class
  bool oneClass = true;
  bool flag = false;
  for (unsigned int i = 0; i < numClasses; i++) {
    if (classFracs[i] > 0) {
      if (flag) {
        oneClass = false;
        break;
      } else {
        flag = true;
      }
    }
  }

  if (oneClass) {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No split necessary, one class already" << std::endl;
#endif
    return makeLeaf(classFracs);
  } else {
    Split split;
    getBestSplit(instances,split);
    std::vector<std::vector<Instance> > splitInstances;
    std::vector<float> splitVals;
    splitData(instances,split,splitInstances,splitVals);
    unsigned int numAcceptable = 0;
    for (unsigned int i = 0; i < splitInstances.size(); i++)
      if (splitInstances[i].size() >= MIN_INSTANCES_PER_LEAF)
        numAcceptable++;
    if ((numAcceptable >= 2) && (split.gain > MIN_GAIN_RATIO)) {

#ifdef DEBUG_DT_SPLITS
      std::cout << "best split = " << featureTypes[split.featureInd].name;
      if (split.numeric)
        std::cout << " < " << split.val;
      std::cout << std::endl;
      std::cout << "best split gain = " << split.gain << std::endl;
#endif
      return makeInterior(instances,split);
    } else {
#ifdef DEBUG_DT_SPLITS
      std::cout << "No useful splits found" << std::endl;
#endif
      return makeLeaf(classFracs);
    }
  }
}

bool DecisionTree::getClassFracs(const std::vector<Instance> &instances, Classification &classFracs) {
  for (unsigned int i = 0; i < instances.size(); i++) {
    Instance const &inst = instances[i];
    int c = (int)(inst[classInd] + 0.5);
    classFracs[c] += inst[weightInd];
  }
  
  float total = 0;
  for (unsigned int i = 0; i < numClasses; i++)
    total += classFracs[i];
  for (unsigned int i = 0; i < numClasses; i++)
    classFracs[i] /= total;
  return (total != 0);
}

void DecisionTree::getBestSplit(const std::vector<Instance> &instances, Split &bestSplit) {
  double I = calcIofSet(instances);
  bestSplit.gain = -1 * std::numeric_limits<float>::infinity();
  Split split;
  // consider splitting on all of the features
  for (unsigned int i = 0; i < features.size(); i++) {
    if ((i == classInd) || (i == weightInd)) //skip the class and weight "features"
      continue;
    split.featureInd = i;
    if (features[i].numeric) {
      // get all of the values for this feature
      std::set<float> vals;
      for (unsigned int j = 0; j < instances.size(); j++)
        vals.insert(instances[j][i]);

      // consider splitting on all of the values for this feature
      std::set<float>::iterator it = vals.begin();
      std::set<float>::iterator it2 = vals.begin();
      if (it != vals.end())
        it++; // drop the first value
      for (; it != vals.end(); it++, it2++) {
        split.val = (*it + *it2) * 0.5;
        //split.val = *it;
        split.gain = calcGainRatio(instances,split,I);
        if (split.gain > 0)
          //std::cout << "considering: " << featureTypes[i].name << " " << split.val << " : " << split.gain <<std::endl;
        if (split.gain > bestSplit.gain)
          bestSplit = split;
      }
    } else {
      split.gain = calcGainRatio(instances,split,I);
      if (split.gain > 0)
        //std::cout << "considering: " << featureTypes[i].name  << " : " << split.gain << std::endl;
      if (split.gain > bestSplit.gain)
        bestSplit = split;
    }
  }
  //std::cout << "best split: " << bestSplit.featureInd << " " << bestSplit.val << " " << bestSplit.gain << std::endl << std::flush;
}
  
double DecisionTree::calcIofSet(const std::vector<Instance> &instances) {
  Classification classFracs(numClasses);
  getClassFracs(instances,classFracs);
  return calcIofP(classFracs);
}

double DecisionTree::calcIofP(const Classification &Pvals) {
  double I = 0;
  for (unsigned int i = 0; i < Pvals.size(); i++) {
    if (Pvals[i] > 0)
      I -= Pvals[i] * log(Pvals[i]);
  }
  return I;
}

double DecisionTree::calcGainRatio(const std::vector<Instance> &instances, const Split &split, double I) {
  //std::cout << "CalcGainRatio for split: " << featureTypes[split.featureInd].name;
  // split the data
  std::vector<std::vector<Instance> > splitInstances;
  std::vector<float> splitVals;
  splitData(instances,split,splitInstances,splitVals);
  //for (unsigned int i = 0; i < splitVals.size(); i++) {
    //std::cout << " " << splitVals[i] << "(" << splitInstances[i].size() << ")";
  //}
  // make sure we have enough data in the splits
  //unsigned int numAcceptable = 0;
  //for (unsigned int i = 0; i < splitInstances.size(); i++)
    //if (splitInstances[i].size() >= MIN_INSTANCES_PER_LEAF)
      //numAcceptable++;
  //if (numAcceptable < 2) {
    ////std::cout << " unacceptable" << std::endl;
    //return -1 * std::numeric_limits<double>::infinity();
  //}
  double info = 0;
  std::vector<float> ratios(splitInstances.size());
  for (unsigned int i = 0; i < splitInstances.size(); i++) {
    ratios[i] = splitInstances[i].size() / (float)instances.size();
    info += ratios[i] * calcIofSet(splitInstances[i]);
    //std::cout << " ratios[" << i << "]:" << ratios[i];
  }
  //std::cout << std::endl;
  //std::cout << " I:" << I << " info:" << info;
  double gain = I - info;
  //std::cout << " gain:" << gain;
  double splitInfo = calcIofP(ratios);
  //std::cout << " splitInfo:" << splitInfo;
  double gainRatio = gain / splitInfo;
  //std::cout << " " << gainRatio;
  //std::cout << std::endl;
  return gainRatio;
}

void DecisionTree::splitData(const std::vector<Instance> &instances, const Split &split, std::vector<std::vector<Instance> > &splitInstances, std::vector<float> &splitVals) {
  Feature &feature = features[split.featureInd];

  if (feature.numeric) {
    splitVals.push_back(split.val);
    splitVals.push_back(std::numeric_limits<float>::infinity());
    //std::cout << " numeric ";
  } else {
    for (unsigned int i = 0; i < feature.values.size(); i++)
      splitVals.push_back(feature.values[i]);
  }
  splitInstances.resize(splitVals.size());

  for (unsigned int i = 0; i < instances.size(); i++) {
    float val = instances[i][split.featureInd];
    if (!feature.numeric)
      val -= 0.5;
    for (unsigned int j = 0; j < splitVals.size(); j++) {
      if (val < splitVals[j]) {
        splitInstances[j].push_back(instances[i]);
        break;
      }
    }
  }
  if (feature.numeric)
    splitVals[1] = splitVals[0];
}
  
boost::shared_ptr<DecisionTree::LeafNode> DecisionTree::makeLeaf(const Classification &classFrac) {
  Classification classification(classFrac);
  if (!useClassDistributions) {
    float maxCount = 0;
    int maxClass = -1;
    for (unsigned int i = 0; i < numClasses; i++) {
      if (classification[i] > maxCount) {
        maxCount = classification[i];
        maxClass = i;
        classification[i] = 0;
      }
    }
    classification[maxClass] = 1;
  }
  return boost::shared_ptr<DecisionTree::LeafNode>(new DecisionTree::LeafNode(classification));
}

boost::shared_ptr<DecisionTree::InteriorNode> DecisionTree::makeInterior(const std::vector<Instance> &instances, const Split &split) {
  Feature &feature = features[split.featureInd];
  ComparisonOperator op = EQUALS;
  if (feature.numeric)
    op = LESS;

  boost::shared_ptr<DecisionTree::InteriorNode> node(new DecisionTree::InteriorNode(op,split.featureInd));

  std::vector<std::vector<Instance> > splitInstances;
  std::vector<float> splitVals;
  splitData(instances,split,splitInstances,splitVals);

  for (unsigned int i = 0; i < splitVals.size(); i++) {
    node->addChild(createNode(splitInstances[i]),splitVals[i]);
  }

  return node;
}

////////////////////
// OUTPUT FUNCTIONS
////////////////////

std::ostream& operator<<(std::ostream &out, boost::shared_ptr<DecisionTree::Node> node) {
  node->genDescription(out);
  return out;
}

std::ostream& operator<<(std::ostream &out, const DecisionTree &dt) {
  dt.root->genDescription(out);
  return out;
}
