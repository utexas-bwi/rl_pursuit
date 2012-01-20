#ifndef SVM_7RSYDUQP
#define SVM_7RSYDUQP

#include "Classifier.h"

namespace libsvm {
#include "libsvm.h"
}

class SVM: public Classifier {
public:
  SVM(const std::vector<Feature> &features, bool caching);
  virtual ~SVM();

  virtual void addData(const InstancePtr &instance);
  virtual void outputDescription(std::ostream &out) const;

protected:
  virtual void trainInternal(bool incremental);
  virtual void classifyInternal(const InstancePtr &instance, Classification &classification);
  void setNode(const InstancePtr &instance, libsvm::svm_node *nodes);
  void createNode(libsvm::svm_node **nodes);
/*
  void scaleInstance(libsvm::svm_node &instance);
  void setScaling();
*/
protected:
  static const int MAX_NUM_INSTANCES = 700000;
  libsvm::svm_problem prob;
  libsvm::svm_model *model;
  libsvm::svm_parameter param;
  libsvm::svm_node *svmInst;

  std::vector<float> minVals;
  std::vector<float> maxVals;
  
  std::vector<float> currentMinVals;
  std::vector<float> currentMaxVals;
};

#endif /* end of include guard: SVM_7RSYDUQP */
