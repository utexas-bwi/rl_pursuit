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

class Communicator;

class WekaClassifier: public Classifier {
public:
  WekaClassifier(const std::vector<Feature> &features, bool caching, const std::string &opts);
  virtual ~WekaClassifier ();
  void addData(const InstancePtr &instance);
  virtual void outputDescription(std::ostream &out) const;

  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);
  virtual void clearData();

  void outputDescriptionToFile(const std::string &filename) const;

  void setDropFrac(float dropFrac) {
    this->dropFrac = dropFrac;
  }

  void saveAsOutput(const std::string &filename) const;

  virtual WekaClassifier* copyWithWeights(const InstanceSet &data);

protected:
  void trainInternal(bool incremental);
  void classifyInternal(const InstancePtr &instance, Classification &classification);

private:
  void writeInstance(const InstanceConstPtr &instance);

  char** splitCommand(const std::string &cmd);
  void freeCommand(char **cmdArr);

  void sendInstances(bool force);

private:
  pid_t pid;
  static const std::string WEKA_CMD;
  
  std::string memSegName;
  boost::shared_ptr<Communicator> comm;
  float dropFrac;
  bool trainAllowed;
  
  static int classifierCount;
};

#endif /* end of include guard: WEKACLASSIFIER_L3XSCUUE */
