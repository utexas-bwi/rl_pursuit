#ifndef PREDATORSTUDENTPYTHON_3BX3WYRK
#define PREDATORSTUDENTPYTHON_3BX3WYRK

/*
File: PredatorStudentPython.h
Author: Samuel Barrett
Description: a wrapper around a student's python predator for assignment 1
Created:  2011-09-12
Modified: 2011-09-12
*/

#include <boost/python.hpp>
#include <string>
#include "Agent.h"

class PredatorStudentPython: public Agent {
public:
  PredatorStudentPython(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd);
  ~PredatorStudentPython();
  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  
  PredatorStudentPython* clone() {
    return new PredatorStudentPython(*this);
  }

private:
  void initClass();
  void cleanClass();

private:
  static const Point2D moves[5];
  static int predatorCount;
  static boost::python::object dictionary;
  boost::python::object predator;
  const std::string name;
};

#endif /* end of include guard: PREDATORSTUDENTPYTHON_3BX3WYRK */
