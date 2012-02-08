#include <iostream>
#include <vector>
#include <learning/Common.h>
#include <learning/ArffReader.h>
#include <factory/ClassifierFactory.h>
#include <factory/AgentFactory.h>
#include <common/Util.h>

void readArff(const std::string &filename, InstanceSet &instances) {
  std::cout << "Reading " << filename << std::endl;
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  while (!arff.isDone()) {
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
    std::cerr << "Usage: boostTest json targetStudent studentList targetDir sourceDir" << std::endl;
    return 1;
  }
  std::string jsonFile = argv[1];
  std::string targetStudent = argv[2];
  std::string studentList = argv[3];
  std::string targetDir = argv[4];
  std::string sourceDir = argv[5];

  Json::Value options;
  if (! readJson(jsonFile,options))
    return 1;
  std::set<std::string> students;
  getAvailableStudents(studentList,students);
  students.erase(targetStudent);
  std::cout << "Using " << students.size() << " source students" << std::endl;
  
  InstanceSet targetData(5);
  readArff(getArffName(targetStudent,targetDir),targetData);
  
  //std::vector<InstanceSet> sourceData;
  //for (std::set<std::string>::iterator it = students.begin(); it != students.end(); it++) {
    //sourceData.push_back(InstanceSet(5));
    //readArff(getArffName(*it,sourceDir),sourceData.back());
  //}
  //std::cout << "Done reading data" << std::endl;
  
  std::string bestStudent = "";
  double bestCorrect = 0.0;
  
  for (std::set<std::string>::iterator it = students.begin(); it != students.end(); it++) {
    Json::Value tempOptions(options);
    tempOptions["filename"] = getDTName(*it,sourceDir);
    ClassifierPtr classifier = createClassifier(tempOptions);
    double fracCorrect;
    double fracMaxCorrect;
    evaluateClassifier(classifier,targetData,fracCorrect,fracMaxCorrect);
    //double &correct = fracCorrect;
    double &correct = fracMaxCorrect;
    std::cout << *it << " " << fracCorrect << " " << fracMaxCorrect << std::endl;

    if (correct > bestCorrect) {
      bestCorrect = correct;
      bestStudent = *it;
    }
  }
  std::cout << "--------------------------" << std::endl;
  std::cout << "Best student: " << bestStudent << std::endl;
  std::cout << "Best correct: " << bestCorrect << std::endl;
  std::cout << "--------------------------" << std::endl;

  //InstanceSet sourceData(5);
  //readArff(getArffName(bestStudent,sourceDir),sourceData);
  options["type"] = "twostagetradaboost";
  options["data"] = getArffName(bestStudent,sourceDir);
  std::cout << "Creating final classifier" << std::endl << std::flush;
  ClassifierPtr finalClassifier = createClassifier(options);
  std::cout << "done creating final classifier" << std::endl << std::flush;
  //for (unsigned int i = 0; i < sourceData.size(); i++)
    //finalClassifier->addSourceData(sourceData[i]);
  for (unsigned int i = 0; i < targetData.size(); i++)
    finalClassifier->addData(targetData[i]);
  finalClassifier->train();
  std::cout << "done training final classifier" << std::endl << std::flush;
  std::cout << *finalClassifier << std::endl;

  return 0;
}
