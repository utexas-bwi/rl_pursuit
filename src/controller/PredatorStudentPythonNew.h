#ifndef PREDATORSTUDENTPYTHONNEW_TPRCRHBL
#define PREDATORSTUDENTPYTHONNEW_TPRCRHBL

/*
File: PredatorStudentPythonNew.h
Author: Samuel Barrett
Description: a wrapper around the 2011 student's python predators
Created:  2011-11-03
Modified: 2011-11-03
*/

#include <boost/python.hpp>
#include <string>
#include "Agent.h"

class PredatorStudentPythonNew: public Agent {
public:
  PredatorStudentPythonNew(const PredatorStudentPythonNew &other);
  PredatorStudentPythonNew(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd);
  ~PredatorStudentPythonNew();
  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  
  PredatorStudentPythonNew* clone() {
    return new PredatorStudentPythonNew(*this);
  }

private:
  void initClass();
  void cleanClass();

private:
  static const char *moveNames[Action::NUM_ACTIONS];
  static const Point2D moves[Action::NUM_ACTIONS];
  static int predatorCount;
  static boost::python::object dictionary;
  boost::python::object predator;
  const std::string name;
};

#endif /* end of include guard: PREDATORSTUDENTPYTHONNEW_TPRCRHBL */
