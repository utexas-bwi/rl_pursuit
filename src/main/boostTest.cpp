#include <iostream>
#include <vector>
#include <boost/foreach.hpp>

#include <learning/Common.h>
#include <learning/ArffReader.h>
#include <learning/TwoStageTrAdaBoost.h>
#include <factory/ClassifierFactory.h>
#include <factory/AgentFactory.h>
#include <common/Util.h>

std::vector<Feature> getFeatures() {
  std::ifstream in("data/dt/blank.arff");
  ArffReader arff(in);
  in.close();
  return arff.getFeatureTypes();
}

void readArff(const std::string &filename, InstanceSet &instances, int numSourceSamples = -1) {
  std::cout << "Reading " << filename << std::endl;
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  while ((!arff.isDone()) && ((numSourceSamples < 0) || ((int)instances.size() < numSourceSamples))) {
    InstancePtr instance = arff.next();
    instances.add(instance);
  }
  in.close();
}

std::string getArffName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/train/" + student + ".arff";
}

std::string getDTName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/weighted/only-" + student + ".weka";
}
 
void evaluateClassifier(ClassifierPtr classifier, const InstanceSet &testData, double &fracCorrect, double &fracMaxCorrect) {
  fracCorrect = 0.0;
  fracMaxCorrect = 0.0;

  for (unsigned int i = 0; i < testData.size(); i++) {
    Classification c;
    classifier->classify(testData[i],c);
    fracCorrect += c[testData[i]->label];
    unsigned int maxInd = vectorMaxInd(c);
    if (maxInd == testData[i]->label)
      fracMaxCorrect++;
  }
  fracCorrect /= testData.size();
  fracMaxCorrect /= testData.size();
}

int main(int argc, const char *argv[]) {
  if (argc != 6) {
    std::cerr << "Expected 5 arguments" << std::endl;
    std::cerr << "Usage: boostTest targetStudent studentList targetDir sourceDir numSourceSamples" << std::endl;
    return 1;
  }
  std::string targetStudent = argv[1];
  std::string studentList = argv[2];
  std::string targetDir = argv[3];
  std::string sourceDir = argv[4];
  int numSourceSamples = atoi(argv[5]);

  //Json::Value options;
  //if (! readJson(jsonFile,options))
    //return 1;
  std::set<std::string> students;
  getAvailableStudents(studentList,students);
  students.erase(targetStudent);
  std::cout << "Using " << students.size() << " source students" << std::endl;
  
  InstanceSet targetData(5);
  readArff(getArffName(targetStudent,targetDir),targetData);
  
  std::vector<std::string> orderedStudents;
  std::vector<double> orderedEvals;
  Json::Value evalOptions;
  evalOptions["type"] = "dt";
  evalOptions["initialTrain"] = false;
  evalOptions["caching"] = false;
  
  for (std::set<std::string>::iterator it = students.begin(); it != students.end(); it++) {
    evalOptions["filename"] = getDTName(*it,sourceDir);
    ClassifierPtr sourceClassifier = createClassifier(evalOptions);
    double fracCorrect;
    double fracMaxCorrect;
    evaluateClassifier(sourceClassifier,targetData,fracCorrect,fracMaxCorrect);
    //double &correct = fracCorrect;
    //double &correct = fracMaxCorrect;
    double correct = 2.0 * fracCorrect * fracMaxCorrect / (fracCorrect + fracMaxCorrect);
    //std::cout << *it << " " << fracCorrect << " " << fracMaxCorrect << std::endl;
    int ind;
    for (ind = 0; ind < (int)orderedEvals.size(); ind++) {
      if (correct > orderedEvals[ind])
        break;
    }
    orderedStudents.insert(orderedStudents.begin() + ind, *it);
    orderedEvals.insert(orderedEvals.begin() + ind, correct);
  }
  for (unsigned int i = 0; i < orderedEvals.size(); i++) {
    std::cout << orderedEvals[i] << " " << orderedStudents[i] << std::endl;
  }
  
  Json::Value baseLearnerOptions;
  baseLearnerOptions["type"] = "weka";
  baseLearnerOptions["caching"] = false;
  baseLearnerOptions["options"] = "weka.classifiers.trees.REPTree";
  ClassifierPtr (*baseLearner)(const std::vector<Feature>&,const Json::Value&) = &createClassifier;
  std::vector<Feature> features = getFeatures();

  std::cout << "Creating classifier" << std::endl << std::flush;
  TwoStageTrAdaBoost classifier(features,false,baseLearner,baseLearnerOptions,5,2,-1);
  std::cout << "done creating classifier" << std::endl << std::flush;
  for (unsigned int i = 0; i < targetData.size(); i++)
    classifier.addData(targetData[i]);
 
  std::vector<float> studentWeights;

  BOOST_FOREACH(std::string &student, orderedStudents) {
    InstanceSet sourceData(5);
    readArff(getArffName(student,sourceDir),sourceData,numSourceSamples);
    for (unsigned int i = 0; i < sourceData.size(); i++)
      classifier.addSourceData(sourceData[i]);
    classifier.train();
    std::cout << "done training classifier" << std::endl << std::flush;
    //std::cout << classifier << std::endl;
    classifier.clearSourceData();
    float sourceInstanceWeight = classifier.getBestSourceInstanceWeight();
    studentWeights.push_back(sourceInstanceWeight);
    if (sourceInstanceWeight > 1e-10) {
      for (unsigned int i = 0; i < sourceData.size(); i++) {
        sourceData[i]->weight = sourceInstanceWeight;
        classifier.addFixedData(sourceData[i]);
      }
    }
  }
  
  for (unsigned int i = 0; i < orderedStudents.size(); i++) {
    std::cout << studentWeights[i] << " " << orderedStudents[i] << std::endl;
  }

  std::cout << classifier << std::endl;

  return 0;
}
