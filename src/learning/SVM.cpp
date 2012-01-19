#include "SVM.h"

SVM::SVM(const std::vector<Feature> &features, bool caching):
  Classifier(features,caching),
  numInstances(0)
{
  // defaults taken from svm_train.c
	param.svm_type = libsvm::C_SVC;
	param.kernel_type = libsvm::RBF;
	param.degree = 3;
	param.gamma = 0;	// 1/num_features
	param.coef0 = 0;
	param.nu = 0.5;
	param.cache_size = 100;
	param.C = 1;
	param.eps = 1e-3;
	param.p = 0.1;
	param.shrinking = 1;
	param.probability = 0;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;
}

SVM::~SVM() {
  for (int i = 0; i < numInstances; i++)
    delete[] instances[i];
}

void SVM::addData(const InstancePtr &instance) {
  assert(numInstances + 1 < MAX_NUM_INSTANCES);
  labels[numInstances] = instance->label;
  instances[numInstances] = new libsvm::svm_node[features.size()];
  setNode(instance,*instances[numInstances]);
  numInstances++;
}

void SVM::outputDescription(std::ostream &out) const {
  // TODO
  out << "SVM" << std::endl;
}

void SVM::trainInternal(bool /*incremental*/) {
  libsvm::svm_problem prob;
  prob.l = numInstances;
  prob.y = labels;
  prob.x = instances;

  libsvm::svm_train(&prob,&param);
}

void SVM::classifyInternal(const InstancePtr &instance, Classification &classification) {
  libsvm::svm_node node;
  setNode(instance,node);
  double label = libsvm::svm_predict(&model,&node);
  unsigned int intLabel = (int)(label + 0.5);
  classification[intLabel] = 1.0;
}

void SVM::setNode(const InstancePtr &instance, libsvm::svm_node &node) {
  for (unsigned int i = 0; i < features.size(); i++) {
    node.index = i;
    node.value = (*instance)[features[i].name];
  }
}
