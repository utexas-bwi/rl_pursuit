#ifndef PREDATORSTUDENTCPP_EUSXHFDS
#define PREDATORSTUDENTCPP_EUSXHFDS

/*
File: PredatorStudentCpp.h
Author: Samuel Barrett
Description: wrapper around the student's c++ predator for assignment 1
Created:  2011-09-14
Modified: 2011-09-14
*/

#include "Agent.h"

class AbstractCppPredator {
public:
  virtual int step(int pos[2], int preyPositions[1][2], int predatorPositions[4][2]) = 0;
};

class PredatorStudentCpp: public Agent {
public:
  PredatorStudentCpp(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd);
  ~PredatorStudentCpp();
  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();

private:
  boost::shared_ptr<AbstractCppPredator> predator;
  const std::string name;
};

#endif /* end of include guard: PREDATORSTUDENTCPP_EUSXHFDS */
