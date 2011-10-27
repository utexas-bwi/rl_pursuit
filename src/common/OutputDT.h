#ifndef OUTPUTDT_UWZJJH2T
#define OUTPUTDT_UWZJJH2T

/*
File: OutputDT.h
Author: Samuel Barrett
Description: outputs information for the decision tree
Created:  2011-10-26
Modified: 2011-10-26
*/

#include <fstream>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include <common/Point2D.h>
#include <controller/Agent.h>

class OutputDT {
public:
  struct FeatureType {
    FeatureType(const std::string &name, int numCategories);
    std::string name;
    int numCategories;
  };
  OutputDT(const std::string &filename, const Point2D &dims, unsigned int numPredators, const std::vector<std::string> &modelNames, bool outputArff, bool useDesiredActions);

  void outputStep(unsigned int numSteps, const Observation &obs, const std::vector<Action::Type> &desiredActions);

protected:
  void outputArffHeader();
  void outputCSVHeader();

protected:
  std::ofstream out;
  Point2D dims;
  unsigned int numPredators;
  std::vector<std::string> modelNames;
  std::vector<boost::shared_ptr<Agent> > models;
  bool outputArff;
  bool useDesiredActions;
  std::vector<FeatureType> features;
  Observation prevObs;
};

#endif /* end of include guard: OUTPUTDT_UWZJJH2T */
