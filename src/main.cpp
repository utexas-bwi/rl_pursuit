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
  char usage[] = "Usage: main [jobNum] config1 [config2 ...]";
  
  if (argc <= 1) {
    std::cerr << "Too few arguments" << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  } else if ((std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help")) {
    std::cout << usage << std::endl;
    return 0;
  }
  
  unsigned int configStart = 1;
  int jobNum = -1;
  // try to interpret the first arg as a job number
  bool isJobNum = true;
  for (int i = 0; argv[1][i] != '\0'; i++) {
    if (!isdigit(argv[1][i])) {
      isJobNum = false;
      break;
    }
  }
  if (isJobNum) {
    jobNum = atoi(argv[1]);
    configStart++;
  }
  

  for (int i = configStart; i < argc; i++) {
    if (! readJson(argv[i],options)) {
      return 1;
    }
  }
  

  unsigned int numTrials = options.get("trials",1).asUInt();
  unsigned int numEpisodes = options.get("numEpisodesPerTrial",1).asUInt();
  unsigned int numTrialsPerJob = options.get("trialsPerJob",1).asUInt();
  bool displayObs = options["verbosity"].get("observation",true).asBool();
  bool displayStepsPerEpisode = options["verbosity"].get("stepsPerEpisode",true).asBool();
  bool displayStepsPerTrial = options["verbosity"].get("stepsPerTrial",true).asBool();

  std::vector<std::vector<unsigned int> > numSteps(numTrials,std::vector<unsigned int>(numEpisodes,0));
  Observation obs;
  double startTime = getTime();

  if (jobNum < 0) {
    jobNum = 0;
  } else {
    numTrials = jobNum + numTrialsPerJob;
  }

  for (unsigned int trial = jobNum; trial < numTrials; trial++) {
    boost::shared_ptr<World> world = createWorldAgents(trial,options);
    boost::shared_ptr<const WorldModel> model = world->getModel();
    if (trial == 0)
      std::cout << world->generateDescription() << std::endl;
    
    if (displayStepsPerTrial)
      std::cout << "trial " << std::setw(2) << trial << ": " << std::flush;
    
    for (unsigned int episode = 0; episode < numEpisodes; episode++) {
      world->randomizePositions();
      world->restartAgents();
      while (!model->isPreyCaptured()) {
        numSteps[trial][episode]++;
        world->step();
        if (displayObs) {
          model->generateObservation(obs);
          std::cout << obs << std::endl;
        }
      } // while the episode lasts
      if (displayStepsPerEpisode)
        std::cout << std::setw(3) << numSteps[trial][episode] << " " << std::flush;
    }
    if (displayStepsPerTrial) {
      unsigned int steps = 0;
      for (unsigned int episode = 0; episode < numEpisodes; episode++)
        steps += numSteps[trial][episode];
      if (displayStepsPerEpisode)
        std::cout << " = ";
      std::cout << std::setprecision(3) << steps / ((float)numEpisodes) << std::endl;
    }
  } // end for trial
  double endTime = getTime();
  std::cout << "Avg Steps Per Episode: ";
  unsigned int numStepsPerEpisode;
  for (unsigned int episode = 0; episode < numEpisodes; episode++) {
    numStepsPerEpisode = 0;
    for (unsigned int trial = 0; trial < numTrials; trial++)
      numStepsPerEpisode += numSteps[trial][episode];
    std::cout << numStepsPerEpisode / ((float)numTrials) << " ";
  }
  std::cout << std::endl;
  std::cout << "time: " << endTime - startTime << std::endl;

  return 0;
}
