#include <iostream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <learning/ArffReader.h>

int main(int argc, const char *argv[]) {
  std::string usage = "Usage: analyzeTrainingData trainingData [file2 ...]";
  if (argc < 2) {
    std::cout << usage << std::endl;
    return 1;
  }
  for (int fileInd = 1; fileInd < argc; fileInd++) {
    std::string trainingName = argv[fileInd];
    std::ifstream trainingFile(trainingName.c_str());
    ArffReader trainingReader(trainingFile);
    std::vector<InstancePtr> instances;
    unsigned int numDups = 0;
    unsigned int numInstances = 0;
    double meanDistToPrey = 0.0;
    double meanDistToTeammates = 0.0;
    
    while (!trainingReader.isDone()) {
      InstancePtr instance = trainingReader.next();
      (*instance)["Trial"] = 0;
      (*instance)["Step"] = 0;

      float distToPrey = fabs((*instance)["Prey.dx"]) + fabs((*instance)["Prey.dy"]);
      meanDistToPrey = numInstances / (numInstances + 1.0) * meanDistToPrey + 1.0 / (numInstances + 1.0) * distToPrey;

      float distToPreds = 0.0;
      for (unsigned int i = 0; i < 4; i++) {
        if (fabs(i - (*instance)["PredInd"]) < 0.1)
          continue;
        std::string key = "Pred" + boost::lexical_cast<std::string>(i) + ".d";
        distToPreds += (fabs((*instance)[key + "x"]) + fabs((*instance)[key + "y"])) / 3.0;
      }
      meanDistToTeammates = numInstances / (numInstances + 1.0) * meanDistToTeammates + 1.0 / (numInstances + 1.0) * distToPreds;

      BOOST_FOREACH(InstancePtr orig, instances) {
        if (*instance == *orig) {
          numDups++;
          break;
        }
      }

      numInstances++;
      //if (numInstances % 1000 == 0)
        //std::cout << numInstances << " " << std::flush;
      instances.push_back(instance);
    }
    //std::cout << std::endl;
    
    std::cout << "-------------------------------------------" << std::endl;
    std::cout << trainingName << std::endl;
    std::cout << "# Instances: " << numInstances << std::endl;
    std::cout << "# Duplicates: " << numDups << std::endl;
    std::cout << "Mean dist to prey: " << meanDistToPrey << std::endl;
    std::cout << "Mean dist to teammates: " << meanDistToTeammates << std::endl;
    
    trainingFile.close();
  }
  return 0;
}
