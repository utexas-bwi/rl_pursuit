#ifndef TWOSTAGETRANSFER_JC2VDL2Z
#define TWOSTAGETRANSFER_JC2VDL2Z

#include <set>
#include "Classifier.h"
#include "SubClassifier.h"
#include "TwoStageTrAdaBoost.h"
#include <rl_pursuit/common/Params.h>

class TwoStageTransfer: public Classifier {
public:
#define PARAMS(_) \
  _(std::string,evalClassifierPath,evalClassifierPath,"") \
  _(std::string,sourceDataPath,sourceDataPath,"") \
  _(std::string,studentList,studentList,"data/newStudents29.txt")\
  _(std::string,targetStudent,targetStudent,"") \
  _(int,numStudentsToAdd,numStudentsToAdd,-1) \
  _(int,maxNumStudents,maxNumStudents,-1)

  Params_STRUCT(PARAMS)
#undef PARAMS

public:
  TwoStageTransfer(const std::vector<Feature> &features, bool caching, SubClassifierGenerator baseLearner, const Json::Value &baseLearnerOptions, SubClassifierGenerator evalGenerator, const Json::Value &evalOptions, const TwoStageTrAdaBoost::Params &baseP, const Params &p);
  
  virtual void addData(const InstancePtr &instance);
  virtual void addSourceData(const InstancePtr &instance);

  virtual void outputDescription(std::ostream &out) const;

  virtual void save(const std::string &filename) const;
  
  virtual bool load(const std::string &filename);
  virtual void clearData() {
    model.clearData();
    targetData.clearData();
  }

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);

  void determineOrdering(std::vector<std::string> &orderedStudents);
  void processStudent(unsigned int ind);
  std::string getEvalPath(const std::string &student) const;
  std::string getDataPath(const std::string &student) const;
  bool fileExists(const std::string &filename);
  void evaluateClassifier(ClassifierPtr classifier, double &fracCorrect, double &fracMaxCorrect) const;
  void readArff(const std::string &student, InstanceSet &data) const;

protected:
  SubClassifierGenerator evalGenerator;
  Json::Value evalOptions;
  TwoStageTrAdaBoost model;
  InstanceSet targetData;
  Params p;
  std::vector<std::string> orderedStudents;
  std::vector<float> studentWeights;
  unsigned int numWeightsDesired;
  bool fullyTrained;
};

#endif /* end of include guard: TWOSTAGETRANSFER_JC2VDL2Z */
