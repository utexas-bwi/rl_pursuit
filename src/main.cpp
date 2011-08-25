/*
File: jsonTest.cpp
Author: Samuel Barrett
Description: tests using json for creating the world
Created:  2011-08-24
Modified: 2011-08-24
*/

#include <string>
#include <iostream>
#include <fstream>
#include <json/json.h>
#include <factory/WorldFactory.h>
#include <common/Util.h>

int main(int argc, const char *argv[])
{
  Json::Value options;
  
  std::string filename = "configs/default.json";
  if (argc > 1)
    filename = argv[1];
  if (! readJson(filename,options))
    return -1;
  
  boost::shared_ptr<World> world = createWorldAgents(0,options);
  boost::shared_ptr<const WorldModel> model = world->getModel();
  bool displayObs = options["verbosity"].get("observation",true).asBool();
  bool stepsPerRun = options["verbosity"].get("stepsPerRun",true).asBool();
  unsigned int numRuns = options.get("runs",1).asUInt();
  unsigned int numSteps;
  float avgSteps = 0;
  Observation obs;

  std::cout << world->generateDescription() << std::endl;

  double startTime = getTime();
  for (unsigned int run = 0; run < numRuns; run++) {
    numSteps = 0;
    world->randomizePositions();
    world->restartAgents();
    while (!model->isPreyCaptured()) {
      numSteps++;
      world->step();
      if (displayObs) {
        model->generateObservation(obs);
        std::cout << obs << std::endl;
      }
    }
    if (stepsPerRun)
      std::cout << run << ": " << numSteps << std::endl;
    avgSteps = (avgSteps * run + numSteps) / (run + 1);
  }
  double endTime = getTime();
  std::cout << "AVG: " << avgSteps << std::endl;
  std::cout << "time: " << endTime - startTime << std::endl;

  return 0;
}
