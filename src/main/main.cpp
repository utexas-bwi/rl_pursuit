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
#include <boost/lexical_cast.hpp>
#include <json/json.h>
#include <factory/WorldFactory.h>
#include <common/Util.h>
#include <learning/OutputDT.h>

void displaySummary(double timePassed, const std::vector<std::vector<unsigned int> > &numSteps);
void displayStepsPerTrial(bool displayStepsPerEpisodeQ, const std::vector<unsigned int> &numStepsPerTrial);
void saveResults(const std::string &filename, int startTrial, const std::vector<std::vector<unsigned int> > &numSteps);
void saveConfig(const Json::Value &options);
void replaceOptsDir(Json::Value &options);
void replaceOptsJob(Json::Value &options, const std::string &jobString);
void replaceOptsTrial(Json::Value &options, unsigned int trialNum);

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

  int numTrials = options.get("trials",1).asUInt();
  int startTrial = 0;
  int origNumTrials = numTrials;
  unsigned int numTrialsPerJob = options.get("trialsPerJob",1).asUInt();
  
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
  replaceOptsDir(options);
  if (jobNum == 0)
    saveConfig(options);

  replaceOptsJob(options,boost::lexical_cast<std::string>(jobNum)); 

  unsigned int numEpisodes = options.get("numEpisodesPerTrial",1).asUInt();
  bool displayDescriptionQ = options["verbosity"].get("description",true).asBool();
  bool displaySummaryQ = options["verbosity"].get("summary",true).asBool();
  bool displayObsQ = options["verbosity"].get("observation",true).asBool();
  bool displayStepsPerEpisodeQ = options["verbosity"].get("stepsPerEpisode",true).asBool();
  bool displayStepsPerTrialQ = options["verbosity"].get("stepsPerTrial",true).asBool();
  std::string saveFilename = options["save"].get("results","").asString();
  bool saveResultsQ = (saveFilename != "");
  bool randomizeSeedQ = options.get("randomizeSeed",false).asBool();

  // get the output DT information
  unsigned int outputDTSteps = options["verbosity"].get("dtsteps",0).asUInt();
  std::string outputDTFilename = options["verbosity"].get("dtfile","").asString();
  bool outputDTCSVQ = (outputDTFilename != "");
  boost::shared_ptr<OutputDT> outputDT;
  boost::shared_ptr<std::vector<Action::Type> > actions;

  Observation obs;
  double startTime = getTime();

  std::vector<std::vector<unsigned int> > numSteps(numTrials,std::vector<unsigned int>(numEpisodes,0));
  std::cout << "Running for " << numTrials << " trials" << std::endl;
  
  unsigned int trialNum;
  unsigned int randomSeed;
  for (int trial = 0; trial < numTrials; trial++) {
    trialNum = trial + startTrial;
    if (randomizeSeedQ)
      randomSeed = getTime() * 1000000 + 1000 * getpid() + trialNum; // hopefully random enough
    else
      randomSeed = trialNum;
    //std::cout << "RANDOM SEED: " << randomSeed << std::endl;

    Json::Value trialOptions(options);
    replaceOptsTrial(trialOptions,trialNum);

    boost::shared_ptr<World> world = createWorldAgents(randomSeed,trialNum,trialOptions);
    boost::shared_ptr<const WorldModel> model = world->getModel();

    // create models for the DT csv output if required
    std::vector<std::string> modelNames;
    if (outputDTCSVQ) {
      if (trial == 0) {
        //modelNames.push_back("GR");
        //modelNames.push_back("TA");
        //modelNames.push_back("GP");
        //modelNames.push_back("PD");
        outputDT = boost::shared_ptr<OutputDT>(new OutputDT(outputDTFilename,model->getDims(),model->getNumAgents()-1,modelNames,true,false,outputDTSteps));
        actions = boost::shared_ptr<std::vector<Action::Type> >(new std::vector<Action::Type>(model->getNumAgents()));
      }

      if (outputDT->hasCollectedSufficientData()) {
        std::cout << "WARNING: collected sufficient data, stopping with " << trial << " trials" << std::endl;
        numSteps.resize(trial);
        break;
      }
    }
    
    if ((trial == 0) && (displayDescriptionQ))
      std::cout << world->generateDescription() << std::endl;
    
    if (displayStepsPerTrialQ)
      std::cout << "trial " << std::setw(2) << trialNum << ": " << std::flush;
    
    for (unsigned int episode = 0; episode < numEpisodes; episode++) {
      world->randomizePositions();
      world->restartAgents();
      while (!model->isPreyCaptured()) {
        numSteps[trial][episode]++;
        if (numSteps[trial][episode] > 10000) {
          std::cerr << "TRIAL " << trial << " EPISODE " << episode << " TOO LONG" << std::endl;
          break;
        }
        //double t = getTime();
        if (displayObsQ) {
          world->generateObservation(obs);
          //std::cout << obs << " " << getTime() - t << std::endl;
          std::cout << obs << std::endl;
        }
        world->step(actions);
        if (outputDTCSVQ) {
          world->generateObservation(obs);
          outputDT->saveStep(trial,numSteps[trial][episode],obs,*actions);
        } // end output dt csv
      } // while the episode lasts
      if (displayObsQ) {
        world->generateObservation(obs);
        //std::cout << obs << " " << getTime() - t << std::endl;
        std::cout << obs << std::endl;
      }
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
  // optionally finialize the saving of data for the DT
  if (outputDTCSVQ)
    outputDT->finalizeSave(randomSeed);

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

void replaceOptsDir(Json::Value &options) {
  std::map<std::string,std::string> reps;
  reps["$(DIR)"] = options.get("dir","").asString();
  jsonReplaceStrings(options,reps);
}

void replaceOptsJob(Json::Value &options, const std::string &jobString) {
  std::map<std::string,std::string> reps;
  reps["$(JOBNUM)"] = jobString;
  Point2D dims = getDims(options);
  std::string size = boost::lexical_cast<std::string>(dims.x) + "x" + boost::lexical_cast<std::string>(dims.y);
  reps["$(SIZE)"] = size;

  jsonReplaceStrings(options,reps);
}

void replaceOptsTrial(Json::Value &options, unsigned int trialNum) {
  std::map<std::string,std::string> reps;
  reps["$(TRIALNUM)"] = boost::lexical_cast<std::string>(trialNum);
  Json::Value const &val = options["predatorOptions"];
  if (!val.isNull()) {
    std::string student = getStudentForTrial(trialNum,val);
    reps["$(STUDENT)"] = student;
  }
  jsonReplaceStrings(options,reps);
}
