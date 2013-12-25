#ifndef OUTPUTDT_UWZJJH2T
#define OUTPUTDT_UWZJJH2T

/*
File: OutputDT.h
Author: Samuel Barrett
Description: outputs information for the decision tree
Created:  2011-10-26
Modified: 2011-12-10
*/

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include <rl_pursuit/common/Point2D.h>
#include "FeatureExtractor.h"

class OutputDT {
public:
  //struct FeatureType {
    //FeatureType(const std::string &name, int numCategories);
    //std::string name;
    //int numCategories;
  //};
  OutputDT(const std::string &filename, const Point2D &dims, unsigned int numPredators, const std::vector<std::string> &modelNames, bool outputArff, bool useDesiredActions, unsigned int numSamples);

  void saveStep(unsigned int trialNum, unsigned int numSteps, const Observation &obs, const std::vector<Action::Type> &desiredActions);

  bool hasCollectedSufficientData();
  void finalizeSave(unsigned int randomSeed);

protected:
  void outputArffHeader();
  void outputCSVHeader();

  void selectInds(unsigned int randomSeed, std::vector<unsigned int> &inds);

protected:
  std::ofstream out;
  std::stringstream ss;
  Point2D dims;
  unsigned int numPredators;
  bool outputArff;
  bool useDesiredActions;
  //std::vector<FeatureType> featureTypes;
  std::vector<std::vector<std::string> > outputForSteps;
  std::vector<Instance> instances;
  Observation prevObs;
  FeatureExtractor featureExtractor;
  std::vector<FeatureExtractorHistory> histories;
  unsigned int numSamples;
};

#endif /* end of include guard: OUTPUTDT_UWZJJH2T */
