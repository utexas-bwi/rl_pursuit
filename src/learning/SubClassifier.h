#ifndef SUBCLASSIFIER_YDWGKH6C
#define SUBCLASSIFIER_YDWGKH6C

/*
File: SubClassifier.h
Author: Samuel Barrett
Description: A sub classifier and some helper functions
Created:  2012-09-10
Modified: 2012-09-10
*/

#include "Classifier.h"
#include <boost/function.hpp>

struct SubClassifier {
  boost::shared_ptr<Classifier> classifier;
  double alpha;
};

typedef boost::function<ClassifierPtr (const std::vector<Feature> &features, const Json::Value &options)> SubClassifierGenerator;

// saving and loading functions
void saveSubClassifiers(const std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<std::string> &subFilenames);
bool createAndLoadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<Feature> &features, const SubClassifierGenerator &baseLearner, const Json::Value &baseLearnerOptions);
bool createAndLoadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<Feature> &features, const std::vector<SubClassifierGenerator> &possibleBaseLearners, const std::vector<Json::Value> &possibleBaseLearnerOptions);

bool loadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename);

void convertWekaToDT(SubClassifier &c);
void convertWekaToDT(ClassifierPtr &c);

#endif /* end of include guard: SUBCLASSIFIER_YDWGKH6C */
