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
  
DecisionTree::InteriorNode::InteriorNode(ComparisonOperator cmp, const std::string &splitKey):
  cmp(cmp),
  splitKey(splitKey)
{
}

void DecisionTree::InteriorNode::addChild(boost::shared_ptr<Node> child, float splitValue) {
  children.push_back(child);
  splitValues.push_back(splitValue);
}

void DecisionTree::InteriorNode::classify(const Features &features, Classification &classification) {
  try {
    //std::cout << "SPLIT on " << splitKey << std::endl;
    unsigned int ind = getInd(features);
    children[ind]->classify(features,classification);
  } catch(std::out_of_range) {
    std::cerr << "DecisionTree::InteriorNode::classify failed for feature: " << splitKey << std::endl;
    exit(5);
  }
}
unsigned int DecisionTree::InteriorNode::getInd(const Features &features) {
  float val = features.at(splitKey);
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
      default:
        std::cerr << "DecisionTree::InteriorNode::classify: ERROR, bad comparison: " << cmp << std::endl;
        exit(5);
        break;
    }
  }
  // was last ind then
  return splitValues.size()-1;
}

std::ostream& DecisionTree::InteriorNode::genDescription(std::ostream &out) {
  out << "<Interior " << splitKey << " " << cmp << std::endl;
  out << "Children: " << std::endl;
  for (unsigned int i = 0; i < splitValues.size(); i++) {
    out << "test : " << splitValues[i] << std::endl;
    out << "child: " << children[i];
  }
  out << ">" << std::endl;
  return out;
}

//////////////////////////////////////////////////////////////
// LEAF NODE
//////////////////////////////////////////////////////////////

DecisionTree::LeafNode::LeafNode(const Classification &classification):
  classification(classification)
{
}

void DecisionTree::LeafNode::classify(const Features &, Classification &classification) {
  classification = this->classification;
}


std::ostream& DecisionTree::LeafNode::genDescription(std::ostream &out) {
  out << "<Leaf " << classification << ">" << std::endl;
  return out;
}


//////////////////////////////////////////////////////////////
// DECISION TREE
//////////////////////////////////////////////////////////////

DecisionTree::DecisionTree(boost::shared_ptr<Node> root):
  root(root)
{
}

void DecisionTree::classify(const Features &features, Classification &classification) {
  root->classify(features,classification);
}


std::ostream& operator<<(std::ostream &out, boost::shared_ptr<DecisionTree::Node> node) {
  node->genDescription(out);
  return out;
}
