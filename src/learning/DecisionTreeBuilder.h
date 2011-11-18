#ifndef DECISIONTREEBUILDER_RALHIB17
#define DECISIONTREEBUILDER_RALHIB17

/*
File: DecisionTreeBuilder.h
Author: Samuel Barrett
Description: trains a decision tree using C4.5
Created:  2011-11-18
Modified: 2011-11-18
*/

#include <boost/shared_ptr.hpp>
#include "DecisionTree.h"

class DecisionTreeBuilder {
public:
  DecisionTreeBuilder(const std::string &filename, unsigned int numClasses, bool useClassDistributions);
  boost::shared_ptr<DecisionTree> makeDecisionTree();
};

#endif /* end of include guard: DECISIONTREEBUILDER_RALHIB17 */
