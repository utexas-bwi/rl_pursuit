#include "SVM.h"
#include <iostream>

void print_null(const char *) {}

SVM::SVM(const std::vector<Feature> &features, bool caching):
  Classifier(features,caching),
  //numInstances(0),
  minVals(features.size()-1,std::numeric_limits<float>::infinity()),
  maxVals(features.size()-1,-1 * std::numeric_limits<float>::infinity()),
  currentMinVals(minVals),
  currentMaxVals(maxVals)
{
  // setup problem
  prob.l = 0;
  prob.y = new double[MAX_NUM_INSTANCES];
  prob.W = new double[MAX_NUM_INSTANCES];
  prob.x = new libsvm::svm_node*[MAX_NUM_INSTANCES];
  createNode(&svmInst);

  // defaults taken from svm_train.c
	param.svm_type = libsvm::C_SVC;
	param.kernel_type = libsvm::RBF;
	param.degree = 3;
	param.gamma = 1.0 / (features.size() - 1);	// 1/num_features
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
  
  // disable libsvm's printing
  libsvm::svm_set_print_string_function(print_null);
}

SVM::~SVM() {
  delete[] prob.y;
  delete[] prob.W;
  for (int i = 0; i < prob.l; i++)
    delete[] prob.x[i];
  delete[] prob.x;

  delete[] svmInst;
  libsvm::svm_free_and_destroy_model(&model);
}

void SVM::addData(const InstancePtr &instance) {
  assert(prob.l + 1 < MAX_NUM_INSTANCES);
  createNode(&prob.x[prob.l]);
  setNode(instance,prob.x[prob.l]);
  prob.y[prob.l] = instance->label;
  prob.W[prob.l] = instance->weight;
  prob.l++;
}

void SVM::outputDescription(std::ostream &out) const {
  // TODO
  out << "SVM" << std::endl;
}

void SVM::trainInternal(bool /*incremental*/) {
  //libsvm::svm_problem prob;
  //prob.l = numInstances;
  //prob.y = labels;
  //prob.x = instances;
  //std::cout << "num instances: " << prob.l << std::endl << std::flush;
  const char *errorMsg = libsvm::svm_check_parameter(&prob,&param);
  if (errorMsg != NULL) {
    std::cerr << "SVM::trainInternal: ERROR from libsvm: " << errorMsg << std::endl;
    exit(13);
  }
  //for (int instInd = 0; instInd < prob.l; instInd++) {
    //assert(prob.x[instInd].dim == (int)features.size() - 1);
    //for (int i = 0; i < prob.x[instInd].dim; i++) {
      //std::cout << prob.x[instInd].values[i] << " ";
    //}
    //std::cout << std::endl;
  //}

  //setScaling();

  model = libsvm::svm_train(&prob,&param);
}

void SVM::classifyInternal(const InstancePtr &instance, Classification &classification) {
  setNode(instance,svmInst);
  //scaleInstance(node);
  double label = libsvm::svm_predict(model,svmInst);
  unsigned int intLabel = (int)(label + 0.5);
  classification[intLabel] = 1.0;
}

void SVM::setNode(const InstancePtr &instance, libsvm::svm_node *nodes) {
  for (unsigned int i = 0; i < features.size() - 1; i++) {
    nodes[i].index = i+1;
    nodes[i].value = (*instance)[features[i].name];
    //if (node.values[i+1] > maxVals[i])
      //maxVals[i] = node.values[i+1];
    //if (node.values[i+1] < minVals[i])
      //minVals[i] = node.values[i+1];
  }
  nodes[features.size()-1].index = -1;
  nodes[features.size()-1].value = 0.0;
}

void SVM::createNode(libsvm::svm_node **nodes) {
  (*nodes) = new libsvm::svm_node[features.size()]; // +1 -1 = 0
}
/*
void SVM::setScaling() {
  // TODO
  static bool firstTime = true;
  assert(firstTime);
  firstTime = false;
  currentMinVals = minVals;
  currentMaxVals = maxVals;
  for (unsigned int i = 0; i < currentMinVals.size(); i++) {
    if (fabs(currentMaxVals[i] - currentMinVals[i]) < 1e-10)
      currentMaxVals[i] += 1;
  }
  std::cout << "RANGES:" << std::endl;
  for (unsigned int i = 0; i < currentMinVals.size(); i++)
    std::cout << currentMinVals[i] << " " << currentMaxVals[i] << std::endl;
  std::cout << "END RANGES" << std::endl;
  
  for (int i = 0; i < prob.l; i++)
    scaleInstance(prob.x[i]);
}
  
void SVM::scaleInstance(libsvm::svm_node &instance) {
  //std::cout << "inst: ";
  for (int i = 1; i < instance.dim; i++) {
    instance.values[i] = (instance.values[i] - currentMinVals[i]) / (currentMaxVals[i] - currentMinVals[i]);
    //std::cout << instance.values[i] << " ";
  }
  //std::cout << std::endl;
}
*/
