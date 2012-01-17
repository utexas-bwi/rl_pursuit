#include "WekaParser.h"
#include <iostream>
#include <cassert>
#include <model/Common.h>
#include "ArffReader.h"

WekaParser::WekaParser(const std::string &filename, unsigned int numClasses):
  in(filename.c_str()),
  numClasses(numClasses)
{
  if (!in.good()) {
    std::cerr << "WekaParser: error opening file: " << filename << std::endl;
    exit(54);
  }
  assert(in.good());
  ArffReader arff(in);
  featureTypes = arff.getFeatureTypes();
  //classFeature = arff.getClassFeature();
  
  //valueMap["U"] = Action::UP;
  //valueMap["D"] = Action::DOWN;
  //valueMap["L"] = Action::LEFT;
  //valueMap["R"] = Action::RIGHT;
  //valueMap["S"] = Action::NOOP;

  tokenizeFile();
}

boost::shared_ptr<DecisionTree> WekaParser::makeDecisionTree(bool caching) {
  for(unsigned int i = 0; i < lines.size(); i++) {
    if (lines[i].name == "")
      lines[i].used = true;
    else
      lines[i].used = false;
  }
  boost::shared_ptr<DecisionTree::Node> root = readDecisionTreeNode(0,0);
  return boost::shared_ptr<DecisionTree>(new DecisionTree(featureTypes,caching,root));
}

boost::shared_ptr<DecisionTree::Node> WekaParser::readDecisionTreeNode(unsigned int lineInd, int currentDepth) {
  Line &line = lines[lineInd];
  //std::cout << lineInd << " " << currentDepth << std::endl;
  if (line.used && !line.leaf) {
    // finished with this line
    //std::cout << "FINISHED THIS LINE" << std::endl;
    return boost::shared_ptr<DecisionTree::Node>();
  }

  //std::cout << line.used << " " << line.leaf << " " << currentDepth << " "  << line.depth + 1 << std::endl;
  // handle leaves specially
  if (line.used && line.leaf && (currentDepth == line.depth + 1)) {
    // make a leaf for this line
    //boost::shared_ptr<DecisionTree::Node> node(new DecisionTree::LeafNode((int)(line.classification + 0.5)));
    InstanceSetPtr instances(new InstanceSet(numClasses));
    instances->classification = line.classDistribution;
    DecisionTree::NodePtr node(new DecisionTree::LeafNode(instances));
    //std::cout << "Making leaf: ";
    //node->output(std::cout,0);
    return node;
  }
  
  // check the depth
  if (currentDepth != line.depth) {
    std::cout << "BAD DEPTH" << std::endl;
    return boost::shared_ptr<DecisionTree::Node>();
  }

 
  line.used = true;
  //std::cout << "MAKING INTERIOR: " << line.name << std::endl;
  boost::shared_ptr<DecisionTree::InteriorNode> node(new DecisionTree::InteriorNode(line.op,line.name));
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

  //std::cout << "DONE MAKING INTERIOR: "  << line.name << std::endl;
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
  
  std::string str = "\n";
  while (str == "\n") {
    str = readWekaToken(true);
  }
  while (in.good()) {
    if (str == "|")
      line.depth++;
    else
      break;
    str = readWekaToken(false);
  }
  if (in.eof()) {
    return;
  }
  if (str == ":") {
    // only a leaf
    line.name = "";
    line.depth--;
  } else {
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
  }

  if (str == ":") {
    // read the class
    line.leaf = true;
    line.classDistribution = Classification(numClasses,0);
    readClass(line);
    //if (useClassDistributions) {
      //for (unsigned int i = 0; i < numClasses; i++) {
        ////std::cout << i << " ";
        //str = readWekaToken(false,true);
        ////std::cout << str << std::endl;
        //line.classDistribution[i] = stringToVal(str,"classification");
      //}
    //} else {
      //str = readWekaToken(false);
      //float val = stringToVal(str,"classification");
      //int ind = (int)(val + 0.5);
      //assert ((ind >= 0) && (ind < (int)numClasses));
      //line.classDistribution[ind] = 1;
    //}
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

std::string WekaParser::readWekaToken(bool acceptNewline, bool breakOnSpace) {
  std::string token;
  std::string spaces;
  bool readingOperator = false;
  char ch;
  while (true) {
    if (in.eof())
      return token;
    ch = in.get();
    if (ch == ' ') {
      if ((breakOnSpace) && (token.size() > 0))
        return token;
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

DecisionTree::ComparisonOperator WekaParser::stringToOperator(const std::string &str) {
  if (str == "=")
    return DecisionTree::EQUALS;
  else if (str == "<")
    return DecisionTree::LESS;
  else if (str == ">=")
    return DecisionTree::GEQ;
  else if (str == "<=")
    return DecisionTree::LEQ;
  else if (str == ">")
    return DecisionTree::GREATER;
  std::cerr << "WekaParser::stringToOperator: ERROR bad operator string: " << str << std::endl;
  exit(5);
}

float WekaParser::stringToVal(const std::string &str, const std::string &) {
  float val;
  try {
    val = boost::lexical_cast<float>(str);
  } catch (boost::bad_lexical_cast) {
    //val = valueMap[name][str];
    //val = valueMap[str];
    std::cout << "str: " << str << std::endl;
    throw;
  }
  //std::cout << "stringToVal(" << str << "," << name << "): " << val << std::endl;
  return val;
}
  
void WekaParser::readClass(Line &line) {
  std::string str;
  std::vector<float> vals;
  for (unsigned int i = 0; i < numClasses; i++) {
    str = readWekaToken(false,true);
    if (str[0] == '(') {
      assert(vals.size() == 1);
      int ind = (int)(vals[0] + 0.5);
      line.classDistribution[ind] = 1;
      return;
    } else {
      vals.push_back(stringToVal(str,"classification"));
    }
  }
  assert(vals.size() == numClasses);
  for (unsigned int  i = 0; i < numClasses; i++)
    line.classDistribution[i] = vals[i];
}
