#ifndef WEKACLASSIFIER_L3XSCUUE
#define WEKACLASSIFIER_L3XSCUUE

/*
File: WekaClassifier.h
Author: Samuel Barrett
Description: interfaces with weka and moa
Created:  2011-12-26
Modified: 2011-12-26
*/

#include "Classifier.h"
#include <fstream>

class WekaClassifier: public Classifier {
public:
  WekaClassifier(const std::vector<Feature> &features, bool caching, const std::string &opts);
  virtual ~WekaClassifier ();
  void addData(const InstancePtr &instance);

protected:
  void trainInternal(bool incremental);
  void classifyInternal(const InstancePtr &instance, Classification &classification);

private:
  void writeFeatures();
  void writeInstance(const InstanceConstPtr &instance);

  char** splitCommand(const std::string &cmd);
  void freeCommand(char **cmdArr);

private:
  std::string toWekaName;
  std::string fromWekaName;
  std::string tempDir;
  std::ofstream out;
  std::ifstream in;
  pid_t pid;
  static const std::string WEKA_CMD;
};

#endif /* end of include guard: WEKACLASSIFIER_L3XSCUUE */
