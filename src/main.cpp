/*
File: main.cpp
Author: Samuel Barrett
Description: tests using json for creating the world
Created:  2011-08-24
Modified: 2011-08-24
*/

#include <string>
#include <iostream>
#include <iomanip>
#include <json/json.h>
#include <factory/WorldFactory.h>
#include <common/Util.h>

int main(int argc, const char *argv[])
{
  Json::Value options;
  char usage[] = "Usage: main config1 [config2 ...]";
  unsigned int configStart = 1;
  
  if (argc <= configStart) {
    std::cerr << "Too few arguments" << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  } else if ((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help")) {
    std::cout << usage << std::endl;
    return 0;
  }
  for (int i = configStart; i < argc; i++) {
    if (! readJson(argv[i],options)) {
      return 1;
    }
  }
  

  unsigned int numTrials = options.get("trials",1).asUInt();
  unsigned int numRuns = options.get("runs",1).asUInt();
  bool displayObs = options["verbosity"].get("observation",true).asBool();
  bool displayStepsPerRun = options["verbosity"].get("stepsPerRun",true).asBool();
  bool displayStepsPerTrial = options["verbosity"].get("stepsPerTrial",true).asBool();

  std::vector<std::vector<unsigned int> > numSteps(numTrials,std::vector<unsigned int>(numRuns,0));
  Observation obs;
  double startTime = getTime();

  for (unsigned int trial = 0; trial < numTrials; trial++) {
    boost::shared_ptr<World> world = createWorldAgents(trial,options);
    boost::shared_ptr<const WorldModel> model = world->getModel();
    if (trial == 0)
      std::cout << world->generateDescription() << std::endl;
    
    if (displayStepsPerTrial)
      std::cout << "trial " << std::setw(2) << trial << ": " << std::flush;
    
    for (unsigned int run = 0; run < numRuns; run++) {
      world->randomizePositions();
      world->restartAgents();
      while (!model->isPreyCaptured()) {
        numSteps[trial][run]++;
        world->step();
        if (displayObs) {
          model->generateObservation(obs);
          std::cout << obs << std::endl;
        }
      } // while the run lasts
      if (displayStepsPerRun)
        std::cout << std::setw(3) << numSteps[trial][run] << " " << std::flush;
    }
    if (displayStepsPerTrial) {
      unsigned int steps = 0;
      for (unsigned int run = 0; run < numRuns; run++)
        steps += numSteps[trial][run];
      if (displayStepsPerRun)
        std::cout << " = ";
      std::cout << std::setprecision(3) << steps / ((float)numRuns) << std::endl;
    }
  } // end for trial
  double endTime = getTime();
  std::cout << "Avg Steps Per Run: ";
  unsigned int numStepsPerRun;
  for (unsigned int run = 0; run < numRuns; run++) {
    numStepsPerRun = 0;
    for (unsigned int trial = 0; trial < numTrials; trial++)
      numStepsPerRun += numSteps[trial][run];
    std::cout << numStepsPerRun / ((float)numTrials) << " ";
  }
  std::cout << std::endl;
  //std::cout << "AVG: " << avgSteps << std::endl;
  std::cout << "time: " << endTime - startTime << std::endl;

  return 0;
}
