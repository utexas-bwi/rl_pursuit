/*
File: DecisionTreeBuilder.cpp
Author: Samuel Barrett
Description: trains a decision tree using C4.5
Created:  2011-11-18
Modified: 2011-11-21
*/

#define DEBUG_DT_SPLITS

#include "DecisionTreeBuilder.h"

#include <iostream>
#include <limits>
#include <set>

const double DecisionTreeBuilder::MIN_GAIN_RATIO = 0.0001;
const unsigned int DecisionTreeBuilder::MIN_INSTANCES_PER_LEAF = 2;

DecisionTreeBuilder::DecisionTreeBuilder(const std::string &filename, unsigned int numClasses, bool useClassDistributions):
  filename(filename),
  numClasses(numClasses),
  useClassDistributions(useClassDistributions)
{
}

boost::shared_ptr<DecisionTree> DecisionTreeBuilder::getDecisionTree() {
  if (dt.get() == NULL) {
    buildTree();
  }
  return dt;
}

void DecisionTreeBuilder::buildTree() {
  std::vector<Features> instances;
  readInstances(instances);
  boost::shared_ptr<DecisionTree::Node> root = createNode(instances);
  dt = boost::shared_ptr<DecisionTree>(new DecisionTree(root));
}

void DecisionTreeBuilder::readInstances(std::vector<Features> &instances) {
  ArffReader arff(filename);
  Features features;
  while (!arff.isDone()) {
    arff.next(features);
    instances.push_back(features);
  }
  std::cout << "Instances: " << instances.size() << std::endl;
  featureTypes = arff.getFeatureTypes();
  classFeature = arff.getClassFeature();
  // remove the class and weight features from the feature list
  std::string ignoreFeatures[4] = {classFeature,WEIGHT_FEATURE,"Trial","Step"};
    for (unsigned int j = 0; j < 4; j++) {
    //if ((featureTypes[i].name == classFeature) || (featureTypes[i].name == WEIGHT_FEATURE)) {
      //featureTypes.erase(featureTypes.begin() + i);
  for (unsigned int i = 0; i < featureTypes.size(); i++) {
      if (featureTypes[i].name == ignoreFeatures[j])
        featureTypes.erase(featureTypes.begin() + i);
    }
  }
  std::cout << "FeatureTypes:" << std::endl;
  for (unsigned int i = 0; i < featureTypes.size(); i++) {
    std::cout << "  " << featureTypes[i].name << std::endl;
  }
}

boost::shared_ptr<DecisionTree::Node> DecisionTreeBuilder::createNode(const std::vector<Features> &instances) {
  ClassCounts classCounts(0);
  getClassCounts(instances,classCounts);
  if (classCounts.size() == 1) {
#ifdef DEBUG_DT_SPLITS
    std::cout << "No split necessary" << std::endl;
#endif
    return makeLeaf(classCounts);
  } else {
    Split split;
    getBestSplit(instances,split);
    std::vector<std::vector<Features> > splitInstances;
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
      return makeLeaf(classCounts);
    }
  }
}

void DecisionTreeBuilder::getClassCounts(const std::vector<Features> &instances, ClassCounts &classCounts) {
  for (unsigned int i = 0; i < instances.size(); i++) {
    Features const &instance = instances[i];
    float val = instance.find(classFeature)->second;
    int c = (int)(val + 0.5);
    classCounts[c]++;
  }
}

void DecisionTreeBuilder::getBestSplit(const std::vector<Features> &instances, Split &bestSplit) {
  double I = calcIofSet(instances);
  bestSplit.gain = -1 * std::numeric_limits<float>::infinity();
  Split split;
  // consider splitting on all of the features
  for (unsigned int i = 0; i < featureTypes.size(); i++) {
    std::cout << "considering: " << featureTypes[i].name << std::endl;
    split.featureInd = i;
    split.numeric = featureTypes[i].numeric;
    if (split.numeric) {
      // get all of the values for this feature
      std::set<float> vals;
      for (unsigned int j = 0; j < instances.size(); j++)
        vals.insert(instances[j].find(featureTypes[i].name)->second);
      // consider splitting on all of the values for this feature
      std::set<float>::iterator it = vals.begin();
      if (it != vals.end())
        it++; // drop the first value
      for (; it != vals.end(); it++) {
        split.val = *it;
        split.gain = calcGainRatio(instances,split,I);
        if (split.gain > bestSplit.gain)
          bestSplit = split;
      }
    } else {
      split.gain = calcGainRatio(instances,split,I);
      if (split.gain > bestSplit.gain)
        bestSplit = split;
    }
  }
  std::cout << "best split: " << bestSplit.featureInd << " " << bestSplit.val << " " << bestSplit.gain << std::endl << std::flush;
}
  
double DecisionTreeBuilder::calcIofSet(const std::vector<Features> &instances) {
  ClassCounts classCounts(0);
  getClassCounts(instances,classCounts);
  std::vector<float> Pvals(numClasses);
  calcClassFracs(classCounts,Pvals);
  return calcIofP(Pvals);
}

double DecisionTreeBuilder::calcIofP(const std::vector<float> &Pvals) {
  double I = 0;
  for (unsigned int i = 0; i < Pvals.size(); i++) {
    if (Pvals[i] > 0)
      I -= Pvals[i] * log(Pvals[i]);
  }
  return I;
}

bool DecisionTreeBuilder::calcClassFracs(const ClassCounts &classCounts, std::vector<float> &classFracs) {
  classFracs.resize(numClasses);
  unsigned int total = 0;
  for (unsigned int i = 0; i < numClasses; i++)
    total += classCounts.get(i);
  for (unsigned int i = 0; i < numClasses; i++)
    classFracs[i] = classCounts.get(i) / (float)total;
  return (total != 0);
}
  
double DecisionTreeBuilder::calcGainRatio(const std::vector<Features> &instances, const Split &split, double I) {
  //std::cout << "CalcGainRatio for split: " << featureTypes[split.featureInd].name;
  // split the data
  std::vector<std::vector<Features> > splitInstances;
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

void DecisionTreeBuilder::splitData(const std::vector<Features> &instances, const Split &split, std::vector<std::vector<Features> > &splitInstances, std::vector<float> &splitVals) {
  ArffReader::Feature &feature = featureTypes[split.featureInd];

  if (split.numeric) {
    splitVals.push_back(split.val);
    splitVals.push_back(std::numeric_limits<float>::infinity());
    //std::cout << " numeric ";
  } else {
    for (unsigned int i = 0; i < feature.values.size(); i++)
      splitVals.push_back(feature.values[i]);
  }
  splitInstances.resize(splitVals.size());

  for (unsigned int i = 0; i < instances.size(); i++) {
    float val = instances[i].find(feature.name)->second;
    if (!feature.numeric)
      val -= 0.5;
    for (unsigned int j = 0; j < splitVals.size(); j++) {
      if (val < splitVals[j]) {
        splitInstances[j].push_back(instances[i]);
        break;
      }
    }
  }
  if (split.numeric)
    splitVals[1] = splitVals[0];
}
  
boost::shared_ptr<DecisionTree::LeafNode> DecisionTreeBuilder::makeLeaf(const ClassCounts &classCounts) {
  std::vector<float> classification(numClasses,0);
  if (useClassDistributions) {
    bool hasData = calcClassFracs(classCounts,classification);
    if (!hasData) {
      for (unsigned int i = 0; i < numClasses; i++)
        classification[i] = 1.0 / numClasses;
    }
  } else {
    unsigned int maxCount = 0;
    int maxClass = -1;
    for (unsigned int i = 0; i < numClasses; i++) {
      if (classCounts.get(i) > maxCount) {
        maxCount = classCounts.get(i);
        maxClass = i;
      }
    }
    classification[maxClass] = 1;
  }
  return boost::shared_ptr<DecisionTree::LeafNode>(new DecisionTree::LeafNode(classification));
}

boost::shared_ptr<DecisionTree::InteriorNode> DecisionTreeBuilder::makeInterior(const std::vector<Features> &instances, const Split &split) {
  ComparisonOperator op = EQUALS;
  if (split.numeric)
    op = LESS;

  ArffReader::Feature &feature = featureTypes[split.featureInd];
  boost::shared_ptr<DecisionTree::InteriorNode> node(new DecisionTree::InteriorNode(op,feature.name));

  std::vector<std::vector<Features> > splitInstances;
  std::vector<float> splitVals;
  splitData(instances,split,splitInstances,splitVals);

  for (unsigned int i = 0; i < splitVals.size(); i++) {
    node->addChild(createNode(splitInstances[i]),splitVals[i]);
  }

  return node;
}
