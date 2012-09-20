#include "TwoStageTransfer.h"
#include <fstream>
#include <boost/foreach.hpp>

#include <factory/AgentFactory.h>
#include "ArffReader.h"

TwoStageTransfer::TwoStageTransfer(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, SubClassifierGenerator evalGenerator, const Json::Value &evalOptions, const TwoStageTrAdaBoost::Params &baseP, const Params &p):
  Classifier(features,caching),
  evalGenerator(evalGenerator),
  evalOptions(evalOptions),
  model(features,caching,baseLearner,baseLearnerOptions,baseP),
  targetData(numClasses),
  p(p),
  fullyTrained(false)
{
}

void TwoStageTransfer::addData(const InstancePtr &instance) {
  model.addData(instance);
  targetData.add(instance);
}

void TwoStageTransfer::addSourceData(const InstancePtr &/*instance*/) {
  //sourceData.add(instance);
  assert(false);
}

void TwoStageTransfer::outputDescription(std::ostream &out) const {
  out << "TwoStageTransfer: ";
  model.outputDescription(out);
}

void TwoStageTransfer::save(const std::string &filename) const {
  std::string orderingFile = getSubFilename(filename,"ordering");

  std::ofstream out(orderingFile.c_str());
  BOOST_FOREACH(const std::string &student, orderedStudents) 
    out << student << std::endl;
  out << "-----" << std::endl;
  BOOST_FOREACH(float weight, studentWeights) 
    out << weight << std::endl;
  out.close();
  
  // only save the model if it's fully trained
  if (fullyTrained)
    model.save(filename);
}

bool TwoStageTransfer::load(const std::string &filename) {
  // see if the complete file can be loaded
  if (fileExists(filename))
    return model.load(filename);
  
  // otherwise, just load the ordering and weights that are already there
  std::string orderingFile = getSubFilename(filename,"ordering");
  std::ifstream in(orderingFile.c_str());
  std::string s;
  in >> s;
  while (in.good() && (s != "-----")) {
    orderedStudents.push_back(s);
    in >> s;
  }
  if (!in.good())
    return false;
  float w;
  in >> w;
  while (in.good()) {
    studentWeights.push_back(w);
    in >> w;
  }
  in.close();
  std::cout << "Loaded " << orderedStudents.size() << " students and " << studentWeights.size() << " weights" << std::endl;
  return true;
}
  
void TwoStageTransfer::trainInternal(bool ) {
  assert(!fullyTrained); // because of the conversion of weka to dt
  // check the necessary parameters
  assert(p.evalClassifierPath != "");
  assert(p.targetStudent != "");
  assert(p.sourceDataPath != "");

  if (orderedStudents.size() == 0)
    determineOrdering(orderedStudents);
  
  // set the num weights desired
  numWeightsDesired = orderedStudents.size();
  unsigned int maxNumStudents = (p.maxNumStudents < 0) ? orderedStudents.size() : p.maxNumStudents;
  unsigned int newNum = studentWeights.size() + p.numStudentsToAdd;
  if ((p.numStudentsToAdd >= 0) && (newNum < numWeightsDesired))
    numWeightsDesired = newNum;
  if (maxNumStudents < numWeightsDesired)
    numWeightsDesired = maxNumStudents;
  if (maxNumStudents < studentWeights.size())
    studentWeights.resize(maxNumStudents);
  
  // process all of the students
  for(unsigned int ind = 0; ind < numWeightsDesired; ind++)
    processStudent(ind);
  
  if (studentWeights.size() == maxNumStudents) {
    model.setTrainFinalModel(true);
    model.train();
    model.convertModelFromWekaToDT();
    fullyTrained = true;
  }
}

void TwoStageTransfer::classifyInternal(const InstancePtr &instance, Classification &classification) {
  model.classify(instance,classification);
}
  
void TwoStageTransfer::determineOrdering(std::vector<std::string> &orderedStudents) {
  std::vector<double> orderedEvals;
  std::set<std::string> students;
  getAvailableStudents(p.studentList,students);
  students.erase(p.targetStudent);
   
  BOOST_FOREACH(const std::string &student, students) {
    ClassifierPtr sourceClassifier = evalGenerator(features,evalOptions);
    sourceClassifier->load(getEvalPath(student));
    double fracCorrect;
    double fracMaxCorrect;
    evaluateClassifier(sourceClassifier,fracCorrect,fracMaxCorrect);
    //double &correct = fracCorrect;
    //double &correct = fracMaxCorrect;
    double correct = 2.0 * fracCorrect * fracMaxCorrect / (fracCorrect + fracMaxCorrect);
    int ind;
    for (ind = 0; ind < (int)orderedEvals.size(); ind++) {
      if (correct > orderedEvals[ind])
        break;
    }
    orderedStudents.insert(orderedStudents.begin() + ind, student);
    orderedEvals.insert(orderedEvals.begin() + ind, correct);
  }
}

std::string TwoStageTransfer::getEvalPath(const std::string &student) const {
  std::string repl = "$(EVAL_STUDENT)";
  size_t ind = p.evalClassifierPath.find(repl);
  assert(ind != std::string::npos);
  std::string res(p.evalClassifierPath);
  return res.replace(ind,repl.size(),student);
}

std::string TwoStageTransfer::getDataPath(const std::string &student) const {
  std::string repl = "$(DATA_STUDENT)";
  size_t ind = p.sourceDataPath.find(repl);
  assert(ind != std::string::npos);
  std::string res(p.sourceDataPath);
  return res.replace(ind,repl.size(),student);
}

void TwoStageTransfer::processStudent(unsigned int ind) {
  if ((ind < studentWeights.size()) && (studentWeights[ind] < 1e-10))
    return;
  InstanceSet sourceData(numClasses);
  readArff(orderedStudents[ind],sourceData);
  if (ind >= studentWeights.size()) {
    for (unsigned int i = 0; i < sourceData.size(); i++)
      model.addSourceData(sourceData[i]);
    model.setTrainFinalModel(false);
    model.train();
    model.clearSourceData();
    studentWeights.push_back(model.getBestSourceInstanceWeight());
  }
  if (studentWeights[ind] < 1e-10)
    return;

  // add the data as fixed
  for (unsigned int i = 0; i < sourceData.size(); i++) {
    sourceData[i]->weight = studentWeights[ind];
    model.addFixedData(sourceData[i]);
  }
}
  
bool TwoStageTransfer::fileExists(const std::string &filename) {
  std::ifstream temp(filename.c_str());
  bool res = temp;
  temp.close();
  return res;
}
  
void TwoStageTransfer::evaluateClassifier(ClassifierPtr classifier, double &fracCorrect, double &fracMaxCorrect) const {
  fracCorrect = 0.0;
  fracMaxCorrect = 0.0;

  for (unsigned int i = 0; i < targetData.size(); i++) {
    Classification c;
    classifier->classify(targetData[i],c);
    fracCorrect += c[targetData[i]->label];
    unsigned int maxInd = vectorMaxInd(c);
    if (maxInd == targetData[i]->label)
      fracMaxCorrect++;
  }
  fracCorrect /= targetData.size();
  fracMaxCorrect /= targetData.size();
}
  
void TwoStageTransfer::readArff(const std::string &student, InstanceSet &data) const {
  std::string filename = getDataPath(student);
  std::cout << "READING " << filename << std::endl;
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  while (!arff.isDone())
    data.add(arff.next());
  in.close();
}
