#include <iostream>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <rl_pursuit/gflags/gflags.h>

#include <rl_pursuit/learning/Common.h>
#include <rl_pursuit/learning/ArffReader.h>
#include <rl_pursuit/learning/TwoStageTrAdaBoost.h>
#include <rl_pursuit/factory/ClassifierFactory.h>
#include <rl_pursuit/factory/AgentFactory.h>
#include <rl_pursuit/common/Util.h>

DEFINE_double(fracSourceData,1.0,"Frac of source data to use");

void addData(const std::string &filename, const ClassifierPtr &c, bool sourceData, boost::shared_ptr<RNG> rng) {
  std::ifstream in(filename.c_str());
  ArffReader arff(in);
  if (sourceData) {
    while (!arff.isDone()) {
      if ((FLAGS_fracSourceData < 1.0 - 1e-10) && (rng->randomFloat() > FLAGS_fracSourceData))
        (void)arff.next();
      else
        c->addSourceData(arff.next());
    }
  } else {
    while (!arff.isDone())
      c->addData(arff.next());
  }
  in.close();
}

int main(int argc, char *argv[]) {
  const char *usage = "Usage: trainClassifier jsonFile saveFile targetData [sourceData1 sourceData2 ...]";
  parseCommandLineArgs(&argc,&argv,usage,3,-1);
  
  int ind = 1;
  const char *jsonFile = argv[ind++];
  const char *saveFile = argv[ind++];
  const char *targetData = argv[ind++];
  int sourceDataStart = ind++;

  Json::Value options;
  if (! readJson(jsonFile,options))
    return 1;
  boost::shared_ptr<RNG> rng(new RNG(time(NULL)));

  ClassifierPtr c = createClassifier(options);
  std::cout << "Created classifier" << std::endl;
  for (int i = sourceDataStart; i < argc; i++) {
    addData(argv[i],c,true,rng);
    std::cout << "Added source data from " << i - sourceDataStart + 1 << " / " << argc - sourceDataStart << ": " << argv[i] << std::endl;
  }
  addData(targetData,c,false,rng);
  std::cout << "Added target data from: " << targetData << std::endl;
  c->train(false);
  std::cout << "Trained" << std::endl;
  if (saveFile != std::string("/dev/null"))
    c->save(saveFile);

  return 0;
}
