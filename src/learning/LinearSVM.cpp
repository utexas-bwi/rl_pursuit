#include "LinearSVM.h"
#include <iostream>
#include <fstream>

namespace liblinear {
void print_null(const char *) {}
}

LinearSVM::LinearSVM(const std::string &filename, const std::vector<Feature> &features, bool caching, unsigned int solverType, unsigned int maxNumInstances):
  Classifier(features,caching),
  MAX_NUM_INSTANCES(maxNumInstances),
  sharedProblem(false),
  minVals(features.size()-1,std::numeric_limits<float>::infinity()),
  maxVals(features.size()-1,-1 * std::numeric_limits<float>::infinity()),
  currentMinVals(minVals),
  currentMaxVals(maxVals)
{
  // setup problem
  prob.l = 0;
  prob.n = features.size() - 1;
  prob.y = new int[MAX_NUM_INSTANCES];
  prob.W = new double[MAX_NUM_INSTANCES];
  prob.x = new liblinear::svm_node*[MAX_NUM_INSTANCES];
  prob.bias = -1;
  createNode(&svmInst);

  // defaults taken from train.c
  param.solver_type = solverType;
  //param.solver_type = liblinear::L2R_LR; // 0.609 // REALLY FAST
  //param.solver_type = liblinear::L2R_L2LOSS_SVC_DUAL; // 0.61
  //param.solver_type = liblinear::L2R_L2LOSS_SVC; // 0.609 // REALLY FAST
  //param.solver_type = liblinear::MCSVM_CS; // 0.65
  //param.solver_type = liblinear::L1R_LR; // 0.609 // REALLY FAST
	param.C = 1;
	param.nr_weight = 0;
	param.weight_label = NULL;
	param.weight = NULL;

  if(param.solver_type == liblinear::L2R_LR || param.solver_type == liblinear::L2R_L2LOSS_SVC)
    param.eps = 0.01;
  else if(param.solver_type == liblinear::L2R_L2LOSS_SVC_DUAL || param.solver_type == liblinear::L2R_L1LOSS_SVC_DUAL || param.solver_type == liblinear::MCSVM_CS || param.solver_type == liblinear::L2R_LR_DUAL)
    param.eps = 0.1;
  else if(param.solver_type == liblinear::L1R_L2LOSS_SVC || param.solver_type == liblinear::L1R_LR)
    param.eps = 0.01;

  
  // disable liblinear's printing
  liblinear::set_print_string_function(liblinear::print_null);

  // load the filename
  if (filename != "")
    load(filename);
}

LinearSVM::LinearSVM(const LinearSVM &svm, bool newWeights):
  Classifier(svm.features,svm.caching),
  MAX_NUM_INSTANCES(svm.MAX_NUM_INSTANCES),
  sharedProblem(true)
{
  assert(newWeights);
  this->prob = svm.prob;
  this->param = svm.param;
  this->prob.W = new double[MAX_NUM_INSTANCES];
  // disable liblinear's printing
  liblinear::set_print_string_function(liblinear::print_null);
  createNode(&svmInst);
}


LinearSVM::~LinearSVM() {
  clearData();

  delete[] svmInst;
  liblinear::free_and_destroy_model(&model);
}

void LinearSVM::addData(const InstancePtr &instance) {
  assert(prob.l < (int)MAX_NUM_INSTANCES);
  createNode(&prob.x[prob.l]);
  setNode(instance,prob.x[prob.l]);
  prob.y[prob.l] = instance->label;
  prob.W[prob.l] = instance->weight;
  prob.l++;
}

void LinearSVM::outputDescription(std::ostream &out) const {
  // TODO
  out << "LinearSVM" << std::endl;
}

void LinearSVM::save(const std::string &filename) const {
  liblinear::save_model(filename.c_str(),model);
}
  
bool LinearSVM::load(const std::string &filename) {
  model = liblinear::load_model(filename.c_str());
  return true;
}
  
void LinearSVM::clearData() {
  if (prob.y == NULL)
    return;
  if (!sharedProblem) {
    delete[] prob.y;
    for (int i = 0; i < prob.l; i++)
      delete[] prob.x[i];
    delete[] prob.x;
  }
  delete[] prob.W;
  prob.y = NULL;
}

void LinearSVM::setWeights(const InstanceSet &data) {
  assert((int)data.size() == prob.l);
  for (unsigned int i = 0; i < data.size(); i++)
    prob.W[i] = data[i]->weight;
}

void LinearSVM::trainInternal(bool /*incremental*/) {
  //liblinear::svm_problem prob;
  //prob.l = numInstances;
  //prob.y = labels;
  //prob.x = instances;
  //std::cout << "num instances: " << prob.l << std::endl << std::flush;
  const char *errorMsg = liblinear::check_parameter(&prob,&param);
  if (errorMsg != NULL) {
    std::cerr << "LinearSVM::trainInternal: ERROR from liblinear: " << errorMsg << std::endl;
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

  model = liblinear::train(&prob,&param);
}

void LinearSVM::classifyInternal(const InstancePtr &instance, Classification &classification) {
  setNode(instance,svmInst);
  //scaleInstance(node);
  double label = liblinear::predict(model,svmInst);
  unsigned int intLabel = (int)(label + 0.5);
  classification[intLabel] = 1.0;
}

void LinearSVM::setNode(const InstancePtr &instance, liblinear::svm_node *nodes) {
  for (unsigned int i = 0; i < features.size() - 1; i++) {
    nodes[i].index = i+1;
    nodes[i].value = (*instance)[features[i].feat];
    //if (node.values[i+1] > maxVals[i])
      //maxVals[i] = node.values[i+1];
    //if (node.values[i+1] < minVals[i])
      //minVals[i] = node.values[i+1];
  }
  nodes[features.size()-1].index = -1;
  nodes[features.size()-1].value = 0.0;
}

void LinearSVM::createNode(liblinear::svm_node **nodes) {
  (*nodes) = new liblinear::svm_node[features.size()]; // +1 -1 = 0
}
/*
void LinearSVM::setScaling() {
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
  
void LinearSVM::scaleInstance(liblinear::svm_node &instance) {
  //std::cout << "inst: ";
  for (int i = 1; i < instance.dim; i++) {
    instance.values[i] = (instance.values[i] - currentMinVals[i]) / (currentMaxVals[i] - currentMinVals[i]);
    //std::cout << instance.values[i] << " ";
  }
  //std::cout << std::endl;
}
*/
