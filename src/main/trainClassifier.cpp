#include <iostream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <learning/Common.h>
#include <learning/ArffReader.h>
#include <learning/TwoStageTrAdaBoost.h>
#include <factory/ClassifierFactory.h>
#include <factory/AgentFactory.h>
#include <common/Util.h>

void addData(const std::string &filename, const ClassifierPtr &c, bool sourceData) {
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  if (sourceData) {
    while (!arff.isDone())
      c->addSourceData(arff.next());
  } else {
    while (!arff.isDone())
      c->addData(arff.next());
  }
  in.close();
}

int main(int argc, const char *argv[]) {
  const char *usage = "Usage: trainClassifier jsonFile saveFile targetData [sourceData1 sourceData2 ...]";
  int numExpectedArgs = 3;

  if (argc < numExpectedArgs + 1) {
    std::cerr << "Expected at least " << numExpectedArgs << " arguments" << std::endl;
    std::cerr << usage << std::endl;
    return 1;
  }
  int ind = 1;
  const char *jsonFile = argv[ind++];
  const char *saveFile = argv[ind++];
  const char *targetData = argv[ind++];
  int sourceDataStart = ind++;

  Json::Value options;
  if (! readJson(jsonFile,options))
    return 1;
  ClassifierPtr c = createClassifier(options);
  std::cout << "Created classifier" << std::endl;
  for (int i = sourceDataStart; i < argc; i++) {
    addData(argv[i],c,true);
    std::cout << "Added source data from " << i - sourceDataStart + 1 << " / " << argc - sourceDataStart << ": " << argv[i] << std::endl;
  }
  addData(targetData,c,false);
  std::cout << "Added target data from: " << targetData << std::endl;
  c->train(false);
  std::cout << "Trained" << std::endl;
  c->save(saveFile);

  return 0;
}
