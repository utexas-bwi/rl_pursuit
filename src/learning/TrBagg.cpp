/*
File: TrBagg.h
Author: Samuel Barrett
Description: implementation of the TrBagg algorithm
Created:  2012-01-18
Modified: 2012-01-18
*/

#include "TrBagg.h"
#include <common/Util.h>
  
TrBagg::TrBagg(const std::vector<Feature> &features, bool caching, BaseLearnerGenerator baseLearner, const Json::Value &baseLearnerOptions, unsigned int maxBoostingIterations, BaseLearnerGenerator fallbackLearner, const Json::Value &fallbackLearnerOptions):
  Classifier(features,caching),
  baseLearner(baseLearner),
  baseLearnerOptions(baseLearnerOptions),
  fallbackLearner(fallbackLearner),
  fallbackLearnerOptions(fallbackLearnerOptions),
  data(numClasses),
  maxBoostingIterations(maxBoostingIterations),
  targetDataStart(-1)
{
}

void TrBagg::addData(const InstancePtr &instance) {
  if (targetDataStart < 0)
    targetDataStart = data.size();
  data.add(instance);
}

void TrBagg::addSourceData(const InstancePtr &instance) {
  assert(targetDataStart < 0);
  data.add(instance);
}

void TrBagg::outputDescription(std::ostream &out) const {
  out << "TrBagg: " << std::endl;
  for (unsigned int i = 0; i < classifiers.size(); i++)
    out << *(classifiers[i].classifier) << std::endl;
}
  
void TrBagg::trainInternal(bool /*incremental*/) {
  classifiers.clear();
  int targetSize = data.size() - targetDataStart;
  assert(targetSize > 0);
  // LEARNING PHASE
  for (unsigned int n = 0; n < maxBoostingIterations; n++) {
    //if (n % 10 == 0)
      //std::cout << "BOOSTING ITERATION: " << n << std::endl;
    BoostingClassifier c;
    c.classifier = baseLearner(features,baseLearnerOptions);
    // sample data set with replacements
    //std::cout << "TRAINING DATA: " << n << std::endl;
    for (int i = 0; i < targetSize; i++) {
      int32_t ind = rng->randomInt(data.size());
      //std::cout << "  " << ind << " " << *data[ind] << std::endl;
      c.classifier->addData(data[ind]);
    }
    c.classifier->train(false);
    //std::cout << "CLASSIFIER: " << *c.classifier << std::endl;
    calcErrorOfClassifier(c);
    // add it to the list of models, but sort the list by error
    std::vector<BoostingClassifier>::iterator it;
    std::vector<BoostingClassifier>::iterator prev = classifiers.begin();
    if ((prev != classifiers.end()) && (prev->alpha > c.alpha)) {
      classifiers.insert(prev,c);
    } else {
      for (it = classifiers.begin(); it != classifiers.end(); it++) {
        if (it->alpha > c.alpha)
          break;
        prev = it;
      }
      classifiers.insert(it,c);
    }
  }
  // create the fallback model
  BoostingClassifier fallbackModel;
  fallbackModel.classifier = fallbackLearner(features,fallbackLearnerOptions);
  for (int i = targetDataStart; i < (int)data.size(); i++)
    fallbackModel.classifier->addData(data[i]);
  calcErrorOfClassifier(fallbackModel);
  // add the fallback model to the beginning of the list
  classifiers.insert(classifiers.begin(),fallbackModel);

  //for (std::vector<BoostingClassifier>::iterator it = classifiers.begin(); it != classifiers.end(); it++) {
    //std::cout << it->alpha << " ";
  //}
  //std::cout << std::endl;
  
  unsigned int bestSize2 = selectSize(classifiers);
  std::cout << "CHOOSING SIZE: " << bestSize2 << std::endl;
  // remove other classifiers
  classifiers.resize(bestSize2);
}

void TrBagg::classifyInternal(const InstancePtr &instance, Classification &classification) {
  classifyInternal(instance,classification,classifiers);
}
  
void TrBagg::classifyInternal(const InstancePtr &instance, Classification &classification, const std::vector<BoostingClassifier> &classifiers) {
  float factor = 1.0 / classifiers.size();
  for (unsigned int j = 0; j < classifiers.size(); j++) {
    Classification temp(numClasses,0);
    classifiers[j].classifier->classify(instance,temp);
    for (unsigned int k = 0; k < numClasses; k++) {
      classification[k] += factor * temp[k];
    }
  }
}
  
void TrBagg::calcErrorOfClassifier(BoostingClassifier &c) {
  std::vector<BoostingClassifier> subset(1,c);
  c.alpha = calcErrorOfSet(subset);
}

double TrBagg::calcErrorOfSet(unsigned int size) {
  std::vector<BoostingClassifier> subset(classifiers.begin(),classifiers.begin() + size);
  return calcErrorOfSet(subset);
}

double TrBagg::calcErrorOfSet(const std::vector<BoostingClassifier> &classifiers) {
  double error = 0.0;
  //std::cout << "  -" << std::endl;
  for (unsigned int i = targetDataStart; i < data.size(); i++) {
    Classification c(numClasses,0);
    classifyInternal(data[i],c,classifiers);
    //std::cout << "  " << *data[i] << " -> " << c[data[i]->label] << std::endl;
    error += fabs(1.0 - c[data[i]->label]);
    //std::cout << "  " << i - targetDataStart << " " << error << std::endl;
  }
  return error;
}
  
unsigned int TrBagg::selectSize(const std::vector<BoostingClassifier> &classifiers) {
  // want to store classification per inst per classifier
  int targetSize = data.size() - targetDataStart;
  assert(targetSize > 0);
  std::vector<std::vector<Classification> > classifications(classifiers.size(),std::vector<Classification>(targetSize,Classification(numClasses,0)));
  for (unsigned int classifierInd = 0; classifierInd < classifiers.size(); classifierInd++) {
    for (int dataInd = 0; dataInd < targetSize; dataInd++) {
      classifiers[classifierInd].classifier->classify(data[dataInd + targetDataStart],classifications[classifierInd][dataInd]);
    }
  }

  unsigned int bestSize = 0;
  double bestError = std::numeric_limits<double>::infinity();
  double err;
  for (unsigned int size = 1; size < classifiers.size(); size++) {
    err = calcErrorOfSet(size,classifications);
    //std::cout << size << ": " << err << std::endl;
    if (err < bestError) {
      bestError = err;
      bestSize = size;
    }
  }
  return bestSize;
}
  
double TrBagg::calcErrorOfSet(unsigned int size, const std::vector<std::vector<Classification> > &classifications) {
  int targetSize = data.size() - targetDataStart;
  float factor = 1.0 / size;
  double err = 0.0;
  //std::cout << "  -" << std::endl;
  for (int dataInd = 0; dataInd < targetSize; dataInd++) {
    unsigned int &label = data[dataInd + targetDataStart]->label;
    err += 1.0; // will reduce by the amount correct in the loop below
    //double temp = err;
    for (unsigned int classifierInd = 0; classifierInd < size; classifierInd++) {
      err -= factor * classifications[classifierInd][dataInd][label];
    }
    //std::cout << "  " << *data[dataInd+targetSize] << " -> " << temp - err << std::endl;
    //std::cout << "  " << dataInd << " " << err << std::endl;
  }
  return err;
}
