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

void DecisionTree::InteriorNode::classify(const Features &features, Classification &classification, bool adjustClassificationQ, unsigned int trueClass) {
  try {
    //std::cout << "SPLIT on " << splitKey << std::endl;
    unsigned int ind = getInd(features);
    children[ind]->classify(features,classification,adjustClassificationQ,trueClass);
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

std::ostream& DecisionTree::InteriorNode::genDescription(std::ostream &out, unsigned int depth) {

  for (unsigned int i = 0; i < splitValues.size(); i++){
    for (unsigned int j = 0; j < depth; j++)
      out << "|  ";
    out << splitKey << " ";
    ComparisonOperator c = cmp;
    if ((c == LESS) && (i == splitValues.size() - 1))
      c = GEQ;
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

//////////////////////////////////////////////////////////////
// LEAF NODE
//////////////////////////////////////////////////////////////

DecisionTree::LeafNode::LeafNode(const Classification &classification):
  classification(classification),
  total(0)
{
}

void DecisionTree::LeafNode::classify(const Features &, Classification &c, bool adjustClassificationQ, unsigned int trueClass) {
  if (adjustClassificationQ)
    adjustClassification(trueClass);
  c = classification;
}

void DecisionTree::LeafNode::adjustClassification(unsigned int trueClass) {
  assert(trueClass < classification.size());
  float frac = total / (total + 1);
  for (unsigned int i = 0; i < classification.size(); i++) {
    if (i == trueClass)
      classification[i] = (classification[i] * total + 1) / (total + 1);
    else
      classification[i] *= frac;
  }
  total += 1;
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


//////////////////////////////////////////////////////////////
// DECISION TREE
//////////////////////////////////////////////////////////////

DecisionTree::DecisionTree(boost::shared_ptr<Node> root):
  root(root)
{
}

void DecisionTree::classify(const Features &features, Classification &classification, bool adjustClassificationQ, unsigned int trueClass) {
  root->classify(features,classification,adjustClassificationQ,trueClass);
}


std::ostream& operator<<(std::ostream &out, boost::shared_ptr<DecisionTree::Node> node) {
  node->genDescription(out);
  return out;
}
