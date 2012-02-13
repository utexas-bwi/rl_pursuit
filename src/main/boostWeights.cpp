#include <iostream>
#include <boost/lexical_cast.hpp>

#include <learning/Common.h>
#include <learning/ArffReader.h>
#include <learning/WekaClassifier.h>
#include <common/Util.h>

std::vector<Feature> getFeatures() {
  std::ifstream in("data/dt/blank.arff");
  ArffReader arff(in);
  in.close();
  return arff.getFeatureTypes();
}

void readAndAddArff(const std::string &filename, Classifier &classifier, float weight = 1.0, int numSourceSamples = -1) {
  std::cout << "Reading " << filename << " with weight/inst: " << weight << std::endl;
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  int counter = 0;
  while ((!arff.isDone()) && ((numSourceSamples < 0) || (counter < numSourceSamples))) {
    InstancePtr instance = arff.next();
    instance->weight = weight;
    classifier.addData(instance);
    counter++;
  }
  in.close();
}

std::string getArffName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/train/" + student + ".arff";
}

std::string getDTName(const std::string &student, const std::string &baseDir) {
  return baseDir + "/weighted/only-" + student + ".weka";
}
 
int main(int argc, const char *argv[]) {
  std::vector<Feature> features = getFeatures();
  std::cout << "Creating classifier" << std::endl << std::flush;
  WekaClassifier classifier(features,false,"weka.classifiers.trees.REPTree");
  int origSourceInstances    =  40000;
  int currentSourceInstances = 200000;

  std::string usage = "Usage: boostWeights targetStudent targetDir sourceDir sourceStudent sourceWeight [sourceStudent sourceWeight ...]";
  if ((argc < 4) || (argc % 2 != 0)) {
    std::cout << usage << std::endl;
  }
  std::cout << "Reweighting source instances as if weights were given for " << origSourceInstances << " and now using " << currentSourceInstances << std::endl;

  std::string targetStudent = argv[1];
  std::string targetDir = argv[2];
  std::string sourceDir = argv[3];
  readAndAddArff(getArffName(targetStudent,targetDir),classifier);
  for (int i = 4; i < argc; i += 2) {
    std::string sourceStudent = argv[i];
    float sourceWeight = boost::lexical_cast<float>(argv[i+1]);
    sourceWeight *= (float)origSourceInstances / (float)currentSourceInstances;
    readAndAddArff(getArffName(sourceStudent,sourceDir),classifier,sourceWeight);
  }
  std::cout << "start training" << std::endl << std::flush;
  classifier.train();
  std::cout << "done  training" << std::endl << std::flush;

  std::cout << classifier << std::endl;

  return 0;
}
