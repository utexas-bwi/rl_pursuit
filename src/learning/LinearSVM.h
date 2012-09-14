#ifndef LinearSVM_7RSYDUQP
#define LinearSVM_7RSYDUQP

#include "Classifier.h"

namespace liblinear {
#include <linear.h>
typedef feature_node svm_node;
typedef problem svm_problem;
typedef model svm_model;
typedef parameter svm_parameter;
}

class LinearSVM: public Classifier {
public:
  LinearSVM(const std::string &filename, const std::vector<Feature> &features, bool caching, unsigned int solverType, unsigned int maxNumInstances);
  LinearSVM(const LinearSVM &svm, bool newWeights);
  virtual ~LinearSVM();

  virtual void addData(const InstancePtr &instance);
  virtual void outputDescription(std::ostream &out) const;
  virtual void save(const std::string &filename) const;
  virtual bool load(const std::string &filename);
  virtual void clearData();
  virtual LinearSVM* copyWithWeights(const InstanceSet &data);

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  void setNode(const InstancePtr &instance, liblinear::svm_node *nodes);
  void createNode(liblinear::svm_node **nodes);
/*
  void scaleInstance(liblinear::svm_node &instance);
  void setScaling();
*/
protected:
  const unsigned int MAX_NUM_INSTANCES;
  liblinear::svm_problem prob;
  liblinear::svm_model *model;
  liblinear::svm_parameter param;
  liblinear::svm_node *svmInst;
  bool sharedProblem;

  std::vector<float> minVals;
  std::vector<float> maxVals;
  
  std::vector<float> currentMinVals;
  std::vector<float> currentMaxVals;
};

#endif /* end of include guard: LinearSVM_7RSYDUQP */
