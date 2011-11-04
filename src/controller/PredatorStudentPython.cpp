/*
File: PredatorStudentPython.cpp
Author: Samuel Barrett
Description: a wrapper around a student's python predator for assignment 1 or the 2011 assignment
Created:  2011-09-12
Modified: 2011-09-12
*/

#include "PredatorStudentPython.h"
#include "PredatorStudentCpp.h"
#include <sys/stat.h>

int PredatorStudentPython::predatorCount = 0;
boost::python::object PredatorStudentPython::dictionary = boost::python::object();
  
PredatorStudentPython::PredatorStudentPython(const PredatorStudentPython &other):
  Agent(other.rng,other.dims),
  isNew(other.isNew)
{
  predatorCount++;

  try {
    boost::python::object deepcopy = dictionary["deepcopy"];
    predator = boost::python::call<boost::python::object>(deepcopy.ptr(),other.predator);
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

PredatorStudentPython::PredatorStudentPython(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd):
  Agent(rng,dims),
  name(name),
  isNew(false)
{
  // initialize the python interpreter if necessary
  if (predatorCount == 0)
    initClass();
  predatorCount++;
  
  // check if we're dealing with a new or old predator
  struct stat buffer;
  isNew = stat(("src/studentAgents/agents/" + name).c_str(),&buffer);

  // load the students predators
  std::ostringstream cmd;
  try {
    if (isNew) {
      cmd << "student = __import__('agentsNew." << name << ".pythonPredator')" << std::endl; // import the right file
      cmd << "predators = student." << name << ".pythonPredator.generatePredators(" << dims << ")" << std::endl; // create the set of predators
      cmd << "predator = predators[" << predatorInd << "]" << std::endl; // select the predator we want
    } else {
      cmd << "student = __import__('agents." << name << "')" << std::endl; // import the right file
      cmd << "predators = student." << name << ".getPredators()" << std::endl; // get the set of predators
      cmd << "predator = predators[" << predatorInd << "]("; // start the constructor for the correct predator
      cmd << dims << ","; // dims
      cmd << "True,"; // toroidalWorld
      // moves
      cmd << "[";
      for (int i = 0; i < 5; i++) {
        cmd << STUDENT_MOVES_OLD[i];
        if (i != 4)
          cmd << ",";
      }
      cmd << "]";
      cmd << ")" << std::endl; // end the constructor
    }
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

  try {
    PyRun_SimpleString(cmd.str().c_str());
    boost::python::object pyObs = dictionary["obs"];
    if (isNew) {
      boost::python::object action;
      boost::python::object pos = pyObs.attr("pos");
      boost::python::object prey = pyObs.attr("preyPositions")[0];
      boost::python::object predPos = pyObs.attr("predatorPositions");
      action = boost::python::call_method<boost::python::object>(predator.ptr(),"step",pos,prey,predPos);
      MoveDistribution moveDist;
      moveDist.probNoop = boost::python::extract<float>(action.attr("probNoop"));
      moveDist.probLeft = boost::python::extract<float>(action.attr("probLeft"));
      moveDist.probRight = boost::python::extract<float>(action.attr("probRight"));
      moveDist.probUp = boost::python::extract<float>(action.attr("probUp"));
      moveDist.probDown = boost::python::extract<float>(action.attr("probDown"));
      return convertStudentActionNew(moveDist);
    } else {
      int action = boost::python::call_method<int>(predator.ptr(),"step",pyObs);
      return convertStudentAction(action);
    }
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

void PredatorStudentPython::restart() {
}

std::string PredatorStudentPython::generateDescription() {
  return "PredatorStudentPython: wrapper around " + name + "'s python predator";
}

void PredatorStudentPython::initClass() {
  Py_Initialize();
  try {
    PyRun_SimpleString("import sys\n");
    PyRun_SimpleString("sys.stdout = open('/dev/null','w')\n"); // disable any output
    PyRun_SimpleString("sys.path.append('src/studentAgents')\n"); // add the studentAgents dir to the path
    PyRun_SimpleString("import random\n"); // import random so we can control the seed
    PyRun_SimpleString("from copy import deepcopy\n"); // import deepcopy for the copy constructor
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
