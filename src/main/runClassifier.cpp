#include <iostream>
#include <string>
#include <factory/ClassifierFactory.h>
#include <common/Util.h>
#include <learning/ArffReader.h>

int main(int argc, const char *argv[]) {
  std::string usage = "Usage: runClassifier optionFile testFile";
  if (argc != 3) {
    std::cout << usage << std::endl;
    return 1;
  }

  std::string optionFile = argv[1];
  std::string testFile   = argv[2];

  Json::Value options;

  if (! readJson(optionFile,options))
    return 1;

  int retrainFreq = options.get("trainingPeriod",-1).asInt();

  ClassifierPtr classifier = createClassifier(options);
  
  std::cout << "------------------------------------------" << std::endl;
  std::cout << *classifier << std::endl;
  if (retrainFreq >= 0)
    std::cout << "Retraining every " << retrainFreq << " steps" << std::endl;
  else
    std::cout << "No retraining" << std::endl;

  std::ifstream testIn(testFile.c_str());
  ArffReader testReader(testIn);
  double correct = 0.0;
  int correctCount = 0;
  int count;
  for (count = 0; !testReader.isDone(); count++) {
  //for (count = 0; count < 10; count++) {
    // retraining
    if ((count > 0) && ((retrainFreq == 0) || ((retrainFreq > 0) && (count % retrainFreq == 0))))
      classifier->train(false);

    InstancePtr instance = testReader.next();
    Classification c;
    classifier->classify(instance,c);
    //std::cout << *instance << std::endl;
    //std::cout << "  ";
    //for (unsigned int i = 0; i < c.size(); i++)
      //std::cout << c[i] << " ";
    //std::cout << std::endl;
    // calculate the fraction correct
    correct += c[instance->label];
    // calculate whether most probable was correct
    float maxVal = -1.0;
    int maxInd = -1;
    for (unsigned int i = 0; i < c.size(); i++) {
      if (c[i] > maxVal) {
        maxVal = c[i];
        maxInd = (int)i;
      }
    }
    if (maxInd == (int)instance->label)
      correctCount++;

    classifier->addData(instance);
  }
  testIn.close();

  //std::cout << *classifier << std::endl;
  //sleep(1); // TODO hack for Weka for now
  std::cout << "------------------------------------------" << std::endl;

  std::cout << "Num Insts   : " << count << std::endl;
  std::cout << "Frac Correct: " << correct << "(" << correct / count << ")" << std::endl;
  std::cout << "Num  Correct: " << correctCount << "(" << correctCount / (float)count << ")" << std::endl;

  return 0;
}
