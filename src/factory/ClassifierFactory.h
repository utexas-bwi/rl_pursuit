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

class DecisionTree;
class WekaClassifier;
class TrAdaBoost;
class NaiveBayes;

ClassifierPtr createClassifier(const Json::Value &options);
ClassifierPtr createClassifier(const std::string &filename, const Json::Value &options);
ClassifierPtr createClassifier(const std::vector<Feature> &features, const Json::Value &options);
ClassifierPtr createClassifier(const std::string &filename, const std::string &dataFilename, const std::vector<Feature> &features, const Json::Value &options);

void addDataToClassifier(ClassifierPtr classifier, const std::string &dataFilename);

boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options);
boost::shared_ptr<DecisionTree> createDecisionTree(const std::string &filename, const std::vector<Feature> &features, bool caching, double minGainRatio, unsigned int minInstancesPerLeaf, int maxDepth);
boost::shared_ptr<DecisionTree> createDecisionTreeFromWeka(const std::string &filename, bool caching);

boost::shared_ptr<WekaClassifier> createWekaClassifier(const std::string &filename, const std::vector<Feature> &features, bool caching, const Json::Value &options);
boost::shared_ptr<TrAdaBoost> createTrAdaBoost(const std::string &filename, std::string &dataFilename, bool caching, const Json::Value &options);

#endif /* end of include guard: CLASSIFIERFACTORY_LSY7J7N1 */
