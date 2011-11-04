#ifndef PREDATORSTUDENTCPP_EUSXHFDS
#define PREDATORSTUDENTCPP_EUSXHFDS

/*
File: PredatorStudentCpp.h
Author: Samuel Barrett
Description: wrapper around the student's c++ predator for assignment 1 and for the second class
Created:  2011-09-14
Modified: 2011-11-04
*/

#include "Agent.h"
#include "PredatorStudentCppAbstract.h"

const Point2D STUDENT_MOVES_OLD[5] = {Point2D(0,0),Point2D(1,0),Point2D(-1,0),Point2D(0,1),Point2D(0,-1)};
ActionProbs convertStudentAction(int action);
ActionProbs convertStudentActionNew(const MoveDistribution &action);

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
  
  static bool handlesStudent(const std::string &name);
  
  PredatorStudentCpp* clone() {
    return new PredatorStudentCpp(*this);
  }

private:
  void createPredator(const std::string &name, unsigned int predatorInd);

private:
  boost::shared_ptr<AbstractCppPredator> predator;
  boost::shared_ptr<PredatorStudentCppAbstract> predatorNew;
  const std::string name;
};

#endif /* end of include guard: PREDATORSTUDENTCPP_EUSXHFDS */
