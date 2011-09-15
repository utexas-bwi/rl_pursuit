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
    unsigned int depth;
    std::string name;
    ComparisonOperator op;
    float val;
    bool leaf;
    float classification;
  };

  WekaParser(const std::string &filename);
  boost::shared_ptr<DecisionTree> makeDecisionTree();
  
private:
  boost::shared_ptr<DecisionTree::Node> readDecisionTreeNode(unsigned int lineInd, unsigned int currentDepth);

  void tokenizeFile();
  void tokenizeLine(Line &line);
  std::string readWekaToken(bool acceptNewline);
  ComparisonOperator stringToOperator(const std::string &str);
  float stringToVal(const std::string &str, const std::string &name);
  

private:
  std::vector<Line> lines;
  std::ifstream in;
  //boost::unordered_map<std::string,Features > valueMap;
  Features valueMap;
};

#endif /* end of include guard: WEKAPARSER_U3AZA2Y8 */
