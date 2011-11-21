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

void DecisionTree::InteriorNode::classify(const Features &features, Classification &classification, bool adjustClassificationQ, unsigned int trueClass, float weight) {
  try {
    //std::cout << "SPLIT on " << splitKey << std::endl;
    unsigned int ind = getInd(features);
    children[ind]->classify(features,classification,adjustClassificationQ,trueClass,weight);
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

void DecisionTree::LeafNode::classify(const Features &, Classification &c, bool adjustClassificationQ, unsigned int trueClass, float weight) {
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

DecisionTree::DecisionTree(boost::shared_ptr<Node> root):
  root(root)
{
}

void DecisionTree::classify(const Features &features, Classification &classification, bool adjustClassificationQ, unsigned int trueClass, float weight) {
  root->classify(features,classification,adjustClassificationQ,trueClass,weight);
}
  
void DecisionTree::randomizeUnseenLeaves() {
  root->randomizeUnseenLeaves();
}

void DecisionTree::generalizeUnseenLeaves() {
  Classification general;
  root->generalizeUnseenLeaves(general);
}

std::ostream& operator<<(std::ostream &out, boost::shared_ptr<DecisionTree::Node> node) {
  node->genDescription(out);
  return out;
}
