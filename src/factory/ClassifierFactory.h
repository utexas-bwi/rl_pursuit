#ifndef CLASSIFIERFACTORY_LSY7J7N1
#define CLASSIFIERFACTORY_LSY7J7N1

/*
File: ClassifierFactory.h
Author: Samuel Barrett
Description: generates classifiers
Created:  2011-12-02
Modified: 2011-12-02
*/

#include <boost/shared_ptr.hpp>
#include <json/json.h>
#include <learning/Classifier.h>
#include <learning/DecisionTree.h>
#include <learning/WekaClassifier.h>

boost::shared_ptr<Classifier> createClassifier(const std::string &filename, const Json::Value &options);
boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::string &dataFilename, const Json::Value &options);
boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::string &dataFilename, double minGainRatio, unsigned int minInstancesPerLeaf, int maxDepth);
boost::shared_ptr<DecisionTree> createDecisionTreeFromWeka(const std::string &filename);
boost::shared_ptr<DecisionTree> createBlankDecisionTreeFromArff(const std::string &dataFilename);
void addDataToDecisionTree(boost::shared_ptr<DecisionTree> dt, const std::string &dataFilename);

boost::shared_ptr<WekaClassifier> createWekaClassifier(const std::string &filename, const std::string &dataFilename, const Json::Value &options);

#endif /* end of include guard: CLASSIFIERFACTORY_LSY7J7N1 */
