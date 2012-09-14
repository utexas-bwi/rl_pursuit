#include "SubClassifier.h"
#include <fstream>

#include "WekaClassifier.h"
#include "DecisionTree.h"
#include <factory/ClassifierFactory.h>

void saveSubClassifiers(const std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<std::string> &subFilenames) {
  std::ofstream out(filename);
  for (unsigned int i = 0; i < classifiers.size(); i++) {
    out << classifiers[i].alpha << " ";
    out << typeid(*(classifiers[i].classifier)).name() << " ";
    out << subFilenames[i] << std::endl;
    classifiers[i].classifier->save(subFilenames[i]);
  }
  out.close();
}

bool createAndLoadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<Feature> &features, const SubClassifierGenerator &baseLearner, const Json::Value &baseLearnerOptions) {
  std::vector<SubClassifierGenerator> learners(1);
  learners[0] = baseLearner;
  std::vector<Json::Value> options(1);
  options[0] = baseLearnerOptions;
  return createAndLoadSubClassifiers(classifiers,filename,features,learners,options);
}

bool createAndLoadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename, const std::vector<Feature> &features, const std::vector<SubClassifierGenerator> &possibleBaseLearners, const std::vector<Json::Value> &possibleBaseLearnerOptions) {
  classifiers.clear();
  // get the names of the base learners
  std::vector<std::string> baseLearnerNames(possibleBaseLearners.size());
  for (unsigned int i = 0; i < possibleBaseLearners.size(); i++) {
    ClassifierPtr temp = possibleBaseLearners[i](features,possibleBaseLearnerOptions[i]);
    baseLearnerNames[i] = typeid(*temp).name();
  }
  // start the loading
  std::string type;
  std::string subFilename;
  std::ifstream in(filename);
  while (in.good()) {
    SubClassifier c;
    in >> c.alpha;
    if (!in.good())
      break;
    in >> type;
    in >> subFilename;
    for (unsigned int i = 0; i < baseLearnerNames.size(); i++) {
      if (type == baseLearnerNames[i]) {
        c.classifier = possibleBaseLearners[i](features,possibleBaseLearnerOptions[i]);
        break;
      }
    }
    if (c.classifier.get() == NULL) {
      in.close();
      std::cerr << "Did not find base learner of type: " << type << std::endl;
      return false;
    }
    if (!c.classifier->load(subFilename))
      return false;
    classifiers.push_back(c);
  }
  in.close();
  return true;
}

bool loadSubClassifiers(std::vector<SubClassifier> &classifiers, const std::string &filename) {
  // start the loading
  std::string type;
  std::string subFilename;
  std::ifstream in(filename);
  unsigned int ind = 0;
  while (in.good()) {
    if (ind + 1 > classifiers.size()) {
      std::cerr << "Loaded too many classifiers, expected " << classifiers.size() << " but currently on " << ind + 1 << std::endl;
      return false;
    }
    in >> classifiers[ind].alpha;
    if (!in.good())
      break;
    in >> type;
    in >> subFilename;
    if (type != typeid(*(classifiers[ind].classifier)).name()) {
      std::cerr << "Expected SubClassifier of type: " << typeid(classifiers[ind]).name() << " but got: " << type << std::endl;
      return false;
    }
    if (!classifiers[ind].classifier->load(subFilename))
      return false;
    ind++;
  }
  in.close();
  if (ind != classifiers.size()) {
    std::cerr << "Loaded too few classifiers, expected " << classifiers.size() << " but got " << ind << std::endl;
    return false;
  }
  return true;
}

void convertWekaToDT(SubClassifier &c) {
  WekaClassifier *temp = dynamic_cast<WekaClassifier*>(c.classifier.get());
  if (temp != NULL) {
    // convert to DT
    std::string filename = tmpnam(NULL);
    temp->saveAsOutput(filename);
    c.classifier = createDecisionTree(filename,temp->getFeatures(),false,Json::Value());
    remove(filename.c_str());
  }
}
