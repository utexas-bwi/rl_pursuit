/*
File: PredatorStudentPython.cpp
Author: Samuel Barrett
Description: a wrapper around a student's python predator for assignment 1
Created:  2011-09-12
Modified: 2011-09-12
*/

#include "PredatorStudentPython.h"

const Point2D PredatorStudentPython::moves[5] = {Point2D(0,0),Point2D(1,0),Point2D(-1,0),Point2D(0,1),Point2D(0,-1)};
int PredatorStudentPython::predatorCount = 0;
boost::python::object PredatorStudentPython::dictionary = boost::python::object();

PredatorStudentPython::PredatorStudentPython(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd):
  Agent(rng,dims),
  name(name)
{
  // initialize the python interpreter if necessary
  if (predatorCount == 0)
    initClass();
  predatorCount++;
  
  // load the students predators
  std::ostringstream cmd;
  try {
    cmd << "student = __import__('studentAgents.agents." << name << "')" << std::endl; // import the right file
    cmd << "predators = student.agents." << name << ".getPredators()" << std::endl; // get the set of predators
    cmd << "predator = predators[" << predatorInd << "]("; // start the constructor for the correct predator
    cmd << dims << ","; // dims
    cmd << "True,"; // toroidalWorld
    // moves
    cmd << "[";
    for (int i = 0; i < 5; i++) {
      cmd << moves[i];
      if (i != 4)
        cmd << ",";
    }
    cmd << "]";
    cmd << ")" << std::endl; // end the constructor
    //std::cout << cmd.str() << std::endl;
    //std::cout << "-------------------" << std::endl;
    PyRun_SimpleString(cmd.str().c_str());
    predator = dictionary["predator"];
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

PredatorStudentPython::~PredatorStudentPython() {
  predatorCount--;
  // cleanup the python interpreter if we're last
  if (predatorCount == 0)
    cleanClass();
}

ActionProbs PredatorStudentPython::step(const Observation &obs) {
  std::ostringstream cmd;
  cmd << "random.seed(" << rng->randomUInt() << ")" << std::endl; // seed the random generator for python
  cmd << "obs = MyObs()" << std::endl;
  cmd << "obs.pos = " << obs.myPos() << std::endl;
  cmd << "obs.preyPositions = [" << obs.preyPos() << "]" << std::endl;
  cmd << "obs.predatorPositions = [";
  for (unsigned int i = 0; i < obs.positions.size(); i++) {
    if ((int)i == obs.preyInd)
      continue;
    cmd << obs.positions[i];
    if (i != obs.positions.size() - 1)
      cmd << ",";
  }
  cmd << "]" << std::endl;
  int step;
  try {
    PyRun_SimpleString(cmd.str().c_str());
    boost::python::object pyObs = dictionary["obs"];
    step = boost::python::call_method<int>(predator.ptr(),"step",pyObs);
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
  return ActionProbs(getAction(moves[step])); //convert the action number to a point, and then convert it to the number -- in case the moves for us and the world don't match up
}

void PredatorStudentPython::restart() {
}

std::string PredatorStudentPython::generateDescription() {
  return "PredatorStudentPython: wrapper around " + name + "'s python predator";
}

void PredatorStudentPython::initClass() {
  Py_Initialize();
  try {
    PyRun_SimpleString("import sys\nsys.stdout = open('/dev/null','w')\n"); // disable any output
    PyRun_SimpleString("sys.path.append('./studentAgents')\n"); // add the studentAgents dir to the path
    PyRun_SimpleString("import random\n"); // import random so we can control the seed
    boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("__main__"))));
    dictionary = module.attr("__dict__");
    PyRun_SimpleString("class MyObs:\n  pass\n");
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

void PredatorStudentPython::cleanClass() {
  Py_Finalize();
  predatorCount = 0;
}
