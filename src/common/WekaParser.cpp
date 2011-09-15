#include "WekaParser.h"
#include <iostream>
#include <cassert>
#include <model/Common.h>

WekaParser::WekaParser(const std::string &filename):
  in(filename.c_str())
{
  assert(in.good());
  valueMap["U"] = Action::UP;
  valueMap["D"] = Action::DOWN;
  valueMap["L"] = Action::LEFT;
  valueMap["R"] = Action::RIGHT;
  valueMap["S"] = Action::NOOP;

  tokenizeFile();
}

boost::shared_ptr<DecisionTree> WekaParser::makeDecisionTree() {
  for(unsigned int i = 0; i < lines.size(); i++)
    lines[i].used = false;
  boost::shared_ptr<DecisionTree::Node> root = readDecisionTreeNode(0,0);
  return boost::shared_ptr<DecisionTree>(new DecisionTree(root));
}

boost::shared_ptr<DecisionTree::Node> WekaParser::readDecisionTreeNode(unsigned int lineInd, unsigned int currentDepth) {
  //std::cout << lineInd << " " << currentDepth << std::endl;
  if (lines[lineInd].used && !lines[lineInd].leaf) {
    // finished with this line
    //std::cout << "FINISHED THIS LINE" << std::endl;
    return boost::shared_ptr<DecisionTree::Node>();
  }

  // handle leaves specially
  if (lines[lineInd].used && lines[lineInd].leaf && (currentDepth == lines[lineInd].depth + 1)) {
    // make a leaf for this line
    boost::shared_ptr<DecisionTree::Node> node(new DecisionTree::LeafNode((int)(lines[lineInd].classification + 0.5)));
    //std::cout << "Making leaf: " << node;
    return node;
  }
  
  // check the depth
  if (currentDepth != lines[lineInd].depth) {
    std::cout << "BAD DEPTH" << std::endl;
    return boost::shared_ptr<DecisionTree::Node>();
  }

 
  lines[lineInd].used = true;
  //std::cout << "MAKING INTERIOR: " << lines[lineInd].name << std::endl;
  boost::shared_ptr<DecisionTree::InteriorNode> node(new DecisionTree::InteriorNode(lines[lineInd].op,lines[lineInd].name));
  boost::shared_ptr<DecisionTree::Node> child;
  for (unsigned int i = lineInd; i < lines.size(); i++) {
    if (lines[i].depth == currentDepth) {
      lines[i].used = true;
      if (lines[i].leaf)
        child = readDecisionTreeNode(i, currentDepth + 1);
      else
        child = readDecisionTreeNode(i + 1, currentDepth + 1);
      node->addChild(child,lines[i].val);
    }
    if (lines[i].depth < currentDepth)
      break;
  }

  //std::cout << "DONE MAKING INTERIOR: "  << lines[lineInd].name << std::endl;
  return node;
}

void WekaParser::tokenizeFile() {
  Line line;
  tokenizeLine(line);
  while (!in.eof()) {
    lines.push_back(line);
    tokenizeLine(line);
  }
  //std::cout << "NUM LINES: " << lines.size() << std::endl;
}

void WekaParser::tokenizeLine(Line &line) {
  //std::cout << "top TokenizeLine" << std::endl;
  line.used = false;
  line.depth = 0;
  
  std::string str;
  while (true) {
    str = readWekaToken(false);
    if (str == "|")
      line.depth++;
    else
      break;
  }
  if (in.eof()) {
    return;
  }
  // read the name
  // don't need to read token, since we did above
  line.name = str;
  // read the operator
  str = readWekaToken(false);
  line.op = stringToOperator(str);
  // read the splitValue
  str = readWekaToken(false);
  line.val = stringToVal(str,line.name);
  // read the rest of the line
  str = readWekaToken(true);
  if (str == ":") {
    // read the class
    line.leaf = true;
    str = readWekaToken(false);
    line.classification = stringToVal(str,"classification");
  } else {
    line.leaf = false;
  }
  while (str != "\n")
    str = readWekaToken(true);

  //std::cout << "LINE: " << line.depth << " " << line.name << " " << line.op << " " << line.val;
  //if (line.leaf)
    //std::cout << " class:" << line.classification;
  //std::cout << std::endl;
}

std::string WekaParser::readWekaToken(bool acceptNewline) {
  std::string token;
  std::string spaces;
  bool readingOperator = false;
  char ch;
  while (true) {
    if (in.eof())
      return token;
    ch = in.get();
    if (ch == ' ') {
      spaces += ch;
      continue;
    }
    if (ch == '\n') {
      if (token.size() == 0) {
        token += ch;
        if (!acceptNewline) {
          std::cerr << "WekaParser::readWekaToken: ERROR unexpected newline" << std::endl;
          exit(5);
        }
      }
      else
        in.unget();
      return token;
    }
    if ((ch == '|') || (ch == ':')) {
      if (token.size() == 0)
        token += ch;
      else
        in.unget();
      return token;
    }

    if (ch == '(') {
      if (token.size() == 0)
        token += ch;
      else {
        in.unget();
        return token;
      }
    }
    if ((ch == '=') || (ch == '<') || (ch == '>')) {
      if (token.size() == 0) { // first character
        readingOperator = true;
        token += ch;
      } else if (readingOperator)
        token += ch;
      else {
        in.unget();
        return token;
      }
    } else if (readingOperator) {
      in.unget();
      return token;
    }
    else {
      if (token.size() > 0)
        token += spaces;
      spaces.clear();
      token += ch;
    }
  }
}

ComparisonOperator WekaParser::stringToOperator(const std::string &str) {
  if (str == "=")
    return EQUALS;
  else if (str == "<")
    return LESS;
  else if (str == ">=")
    return GEQ;
  std::cerr << "WekaParser::stringToOperator: ERROR bad operator string: " << str << std::endl;
  exit(5);
}

float WekaParser::stringToVal(const std::string &str, const std::string &) {
  float val;
  try {
    val = boost::lexical_cast<float>(str);
  } catch (boost::bad_lexical_cast) {
    //val = valueMap[name][str];
    val = valueMap[str];
  }
  //std::cout << "stringToVal(" << str << "," << name << "): " << val << std::endl;
  return val;
}
