/*
File: PredatorStudentPythonNew.cpp
Author: Samuel Barrett
Description: a wrapper around the 2011 student's python predators
Created:  2011-09-12
Modified: 2011-09-12
*/

#include "PredatorStudentPythonNew.h"
    
const char *PredatorStudentPythonNew::moveNames[Action::NUM_ACTIONS] = {"probNoop","probLeft","probRight","probUp","probDown"};
const Point2D PredatorStudentPythonNew::moves[Action::NUM_ACTIONS] = {Point2D(0,0),Point2D(-1,0),Point2D(1,0),Point2D(0,-1),Point2D(0,1)};

int PredatorStudentPythonNew::predatorCount = 0;
boost::python::object PredatorStudentPythonNew::dictionary = boost::python::object();
  
PredatorStudentPythonNew::PredatorStudentPythonNew(const PredatorStudentPythonNew &other):
  Agent(other.rng,other.dims)
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

PredatorStudentPythonNew::PredatorStudentPythonNew(boost::shared_ptr<RNG> rng, const Point2D &dims, const std::string &name, unsigned int predatorInd):
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
    cmd << "student = __import__('agentsNew." << name << ".pythonPredator')" << std::endl; // import the right file
    cmd << "predators = student." << name << ".pythonPredator.generatePredators([" << dims.x << "," << dims.y << "])" << std::endl; // create the set of predators
    cmd << "predator = predators[" << predatorInd << "]" << std::endl; // select the predator we want
    PyRun_SimpleString(cmd.str().c_str());
    predator = dictionary["predator"];
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

PredatorStudentPythonNew::~PredatorStudentPythonNew() {
  predatorCount--;
  // cleanup the python interpreter if we're last
  if (predatorCount == 0)
    cleanClass();
}

ActionProbs PredatorStudentPythonNew::step(const Observation &obs) {
  std::ostringstream cmd;
  cmd << "random.seed(" << rng->randomUInt() << ")" << std::endl; // seed the random generator for python
  cmd << "pos = " << obs.myPos() << std::endl;
  cmd << "preyPosition = " << obs.preyPos() << std::endl;
  cmd << "predatorPositions = [";
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

    boost::python::object action;
    boost::python::object pos = dictionary["pos"];
    boost::python::object prey = dictionary["preyPosition"];
    boost::python::object predPos = dictionary["predatorPositions"];
    action = boost::python::call_method<boost::python::object>(predator.ptr(),"step",pos,prey,predPos);

    ActionProbs actionProbs;
    for (int a = 0; a < Action::NUM_ACTIONS; a++) {
      float prob = boost::python::extract<float>(action.attr(moveNames[a]));
      actionProbs[getAction(moves[a])] = prob;
    }
    return actionProbs;

    //return ActionProbs(getAction(moves[step])); //convert the action number to a point, and then convert it to the number -- in case the moves for us and the world don't match up
  } catch (boost::python::error_already_set) {
    PyErr_Print();
    throw;
  }
}

void PredatorStudentPythonNew::restart() {
}

std::string PredatorStudentPythonNew::generateDescription() {
  return "PredatorStudentPythonNew: wrapper around " + name + "'s python predator";
}

void PredatorStudentPythonNew::initClass() {

  Py_Initialize();
  try {
    PyRun_SimpleString("import sys\n");
    //PyRun_SimpleString("sys.stdout = open('/dev/null','w')\n"); // disable any output
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

void PredatorStudentPythonNew::cleanClass() {
  Py_Finalize();
  predatorCount = 0;
}
