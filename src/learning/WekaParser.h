#ifndef WEKAPARSER_U3AZA2Y8
#define WEKAPARSER_U3AZA2Y8

/*
File: WekaParser.h
Author: Samuel Barrett
Description: parses a weka tree into a decision tree
Created:  2011-09-13
Modified: 2011-09-13
*/

#include <vector>
#include <string>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/unordered_map.hpp>
#include "DecisionTree.h"
#include <model/Common.h>

class WekaParser {
public:
  struct Line {
    bool used;
    int depth;
    std::string name;
    DecisionTree::ComparisonOperator op;
    float val;
    bool leaf;
    Classification classDistribution;
  };

  WekaParser(const std::string &filename, unsigned int numClasses);
  boost::shared_ptr<DecisionTree> makeDecisionTree(bool caching);
  boost::shared_ptr<DecisionTree::Node> makeTreeRoot();
  
private:
  boost::shared_ptr<DecisionTree::Node> readDecisionTreeNode(unsigned int lineInd, int currentDepth);

  void tokenizeFile();
  void tokenizeLine(Line &line);
  std::string readWekaToken(bool acceptNewline, bool breakOnSpace = false);
  DecisionTree::ComparisonOperator stringToOperator(const std::string &str);
  float stringToVal(const std::string &str, const std::string &name);
  void readClass(Line &line);
  

private:
  std::vector<Line> lines;
  std::ifstream in;
  //boost::unordered_map<std::string,Features > valueMap;
  unsigned int numClasses;
  std::vector<Feature> featureTypes;
  std::string classFeature;
};

#endif /* end of include guard: WEKAPARSER_U3AZA2Y8 */
