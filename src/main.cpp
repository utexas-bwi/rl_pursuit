/*
File: main.cpp
Author: Samuel Barrett
Description: tests using json for creating the world
Created:  2011-08-24
Modified: 2011-08-24
*/

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <json/json.h>
#include <factory/WorldFactory.h>
#include <common/Util.h>

void displaySummary(double timePassed, const std::vector<std::vector<unsigned int> > &numSteps);
void displayStepsPerTrial(bool displayStepsPerEpisodeQ, const std::vector<unsigned int> &numStepsPerTrial);
void saveResults(const std::string &filename, int startTrial, const std::vector<std::vector<unsigned int> > &numSteps);
void saveConfig(const Json::Value &options);

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
  std::string jobString = "";
  // try to interpret the first arg as a job number
  bool isJobNum = true;
  for (int i = 0; argv[1][i] != '\0'; i++) {
    if (!isdigit(argv[1][i])) {
      isJobNum = false;
      break;
    }
  }
  if (isJobNum) {
    jobString = argv[1];
    jobNum = atoi(argv[1]);
    configStart++;
  }
  

  for (int i = configStart; i < argc; i++) {
    if (! readJson(argv[i],options)) {
      return 1;
    }
  }
  
  int numTrials = options.get("trials",1).asUInt();
  unsigned int numEpisodes = options.get("numEpisodesPerTrial",1).asUInt();
  unsigned int numTrialsPerJob = options.get("trialsPerJob",1).asUInt();
  bool displayDescriptionQ = options["verbosity"].get("description",true).asBool();
  bool displaySummaryQ = options["verbosity"].get("summary",true).asBool();
  bool displayObsQ = options["verbosity"].get("observation",true).asBool();
  bool displayStepsPerEpisodeQ = options["verbosity"].get("stepsPerEpisode",true).asBool();
  bool displayStepsPerTrialQ = options["verbosity"].get("stepsPerTrial",true).asBool();
  std::string saveFilename = options["save"].get("results","").asString();
  bool saveResultsQ = (saveFilename != "");
  if (saveResultsQ) {
    size_t pos = saveFilename.find("$(JOBNUM)");
    if (pos != std::string::npos) {
      saveFilename.replace(pos,9,jobString);
    }
  }

  Observation obs;
  double startTime = getTime();
  int startTrial = 0;
  int origNumTrials = numTrials;

  if (jobNum < 0) {
    jobNum = 0;
  } else {
    startTrial = jobNum * numTrialsPerJob;
    numTrials = min((int)numTrialsPerJob,numTrials-startTrial);
  }
  if (numTrials <= 0) {
    std::cerr << "ERROR: insufficient number of trials: " << numTrials << std::endl;
    std::cerr << "Calculated from: jobNum: " << jobNum << " numTrialsPerJob: " << numTrialsPerJob << " numTrials: " << origNumTrials << std::endl;
    std::cerr << "Start trial should be: " << startTrial << std::endl;
    return 1;
  }
  if (jobNum == 0)
    saveConfig(options);

  std::vector<std::vector<unsigned int> > numSteps(numTrials,std::vector<unsigned int>(numEpisodes,0));
  std::cout << "Running for " << numTrials << " trials" << std::endl;
  
  unsigned int trialNum;
  for (int trial = 0; trial < numTrials; trial++) {
    trialNum = trial + startTrial;
    boost::shared_ptr<World> world = createWorldAgents(trialNum,options);
    boost::shared_ptr<const WorldModel> model = world->getModel();
    if ((trial == 0) && (displayDescriptionQ))
      std::cout << world->generateDescription() << std::endl;
    
    if (displayStepsPerTrialQ)
      std::cout << "trial " << std::setw(2) << trialNum << ": " << std::flush;
    
    for (unsigned int episode = 0; episode < numEpisodes; episode++) {
      world->randomizePositions();
      world->restartAgents();
      while (!model->isPreyCaptured()) {
        numSteps[trial][episode]++;
        world->step();
        if (displayObsQ) {
          model->generateObservation(obs);
          std::cout << obs << std::endl;
        }
      } // while the episode lasts
      if (displayStepsPerEpisodeQ)
        std::cout << std::setw(3) << numSteps[trial][episode] << " " << std::flush;
    }
    if (displayStepsPerTrialQ)
      displayStepsPerTrial(displayStepsPerEpisodeQ,numSteps[trial]);
  } // end for trial
  double endTime = getTime();
  // optionally display the summary
  if (displaySummaryQ)
    displaySummary(endTime-startTime,numSteps);
  // optionally save the results
  if (saveResultsQ)
    saveResults(saveFilename,startTrial,numSteps);

  return 0;
}


void displaySummary(double timePassed, const std::vector<std::vector<unsigned int> > &numSteps) {
  std::cout << "Avg Steps Per Episode: ";
  unsigned int numStepsPerEpisode;
  for (unsigned int episode = 0; episode < numSteps[0].size(); episode++) {
    numStepsPerEpisode = 0;
    for (unsigned int trial = 0; trial < numSteps.size(); trial++)
      numStepsPerEpisode += numSteps[trial][episode];
    std::cout << numStepsPerEpisode / ((float)numSteps.size()) << " ";
  }
  std::cout << std::endl;
  std::cout << "time: " << timePassed << std::endl;
}

void displayStepsPerTrial(bool displayStepsPerEpisodeQ, const std::vector<unsigned int> &numStepsPerTrial) {
  unsigned int numEpisodes = numStepsPerTrial.size();
  unsigned int steps = 0;
  for (unsigned int episode = 0; episode < numEpisodes; episode++)
    steps += numStepsPerTrial[episode];
  if (displayStepsPerEpisodeQ)
    std::cout << " = ";
  std::cout << std::setprecision(3) << steps / ((float)numEpisodes) << std::endl;
}

void saveResults(const std::string &filename, int startTrial, const std::vector<std::vector<unsigned int> > &numSteps) {
  std::ofstream out(filename.c_str());
  if (!out.good()) {
    std::cerr << "ERROR: error saving to " << filename << std::endl;
    out.close();
    return;
  }
  for (unsigned int trial = 0; trial < numSteps.size(); trial++) {
    out << trial + startTrial;
  for (unsigned int episode = 0; episode < numSteps[trial].size(); episode++)
      out << "," << numSteps[trial][episode];
    out << std::endl;
  }
  out.close();
}

void saveConfig(const Json::Value &options) {
  std::string saveConfigFilename = options["save"].get("config","").asString();
  if (saveConfigFilename == "")
    return;
  
  std::ofstream destFile(saveConfigFilename.c_str());
  if (! destFile.good()) {
    std::cerr << "WARNING: not saving config file due to problem opening destination file: " << saveConfigFilename << std::endl;
    return;
  }

  Json::StyledStreamWriter writer("  ");
  writer.write(destFile,options);
  destFile.close();
}
