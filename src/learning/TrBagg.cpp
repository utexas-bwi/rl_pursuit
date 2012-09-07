/*
File: TrBagg.h
Author: Samuel Barrett
Description: implementation of the TrBagg algorithm
Created:  2012-01-18
Modified: 2012-01-18
*/

#include "TrBagg.h"
#include <common/Util.h>
#include <fstream>
  
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

void TrBagg::save(const std::string &filename) const {
  std::ofstream out(filename);
  for (unsigned int i = 0; i < classifiers.size(); i++) {
    std::string subFilename = getSubFilename(filename,i);
    out << classifiers[i].alpha << " ";
    out << typeid(*(classifiers[i].classifier)).name() << " ";
    out << subFilename << std::endl;
    classifiers[i].classifier->save(subFilename);
  }

  out.close();
}
  
bool TrBagg::load(const std::string &filename) {
  std::string type;
  std::string subFilename;
  std::ifstream in(filename);
  ClassifierPtr x = baseLearner(features,baseLearnerOptions);
  std::string basename = typeid(*x).name();
  x = fallbackLearner(features,fallbackLearnerOptions);
  std::string fallbackname = typeid(*x).name();
  while (in.good()) {
    BoostingClassifier c;
    in >> c.alpha;
    in >> type;
    in >> subFilename;
    if (type == basename)
      c.classifier = baseLearner(features,baseLearnerOptions);
    else if (type == fallbackname)
      c.classifier = fallbackLearner(features,fallbackLearnerOptions);
    else {
      std::cerr << "Expected a subclass of either " << basename << " or " << fallbackname << " but got: " << type << std::endl;
      return false;
    }
    if (!c.classifier->load(subFilename))
      return false;
    classifiers.push_back(c);
  }
  in.close();
  std::cout << "size: " << classifiers.size() << std::endl;
  return true;
}
  
void TrBagg::trainInternal(bool /*incremental*/) {
  classifiers.clear();
  if (targetDataStart < 0) {
    std::cerr << "WARNING: Trying to train TrBagg with no target data, just falling back on the fallback learner applied to the source data" << std::endl;
    BoostingClassifier fallbackModel;
    fallbackModel.classifier = fallbackLearner(features,fallbackLearnerOptions);
    for (unsigned int i = 0; i < data.size(); i++)
      fallbackModel.classifier->addData(data[i]);
    fallbackModel.classifier->train(false);
    fallbackModel.alpha = 1.0;
    classifiers.push_back(fallbackModel);
    return;
  }
  
  //std::cout << "targetDataStart: " << targetDataStart << "  " << "data.size: " << data.size() << std::endl;
  int targetSize = data.size() - targetDataStart;
  assert(targetSize > 0);
  int sampleSize = 1 * targetSize;
  //int sampleSize = 1000;
  // LEARNING PHASE
  for (unsigned int n = 0; n < maxBoostingIterations; n++) {
    //if (n % 10 == 0)
      //std::cout << "BOOSTING ITERATION: " << n << std::endl;
    BoostingClassifier c;
    c.classifier = baseLearner(features,baseLearnerOptions);
    // sample data set with replacements
    //std::cout << "TRAINING DATA: " << n << std::endl;
    for (int i = 0; i < sampleSize; i++) {
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
  if (targetDataStart >= 0)  {
    for (int i = targetDataStart; i < (int)data.size(); i++)
      fallbackModel.classifier->addData(data[i]);
  }
  fallbackModel.classifier->train(false);
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
    //std::cout << *(classifiers[size-1].classifier) << std::endl;
    err = calcErrorOfSet(size,classifications);
    //std::cout << size << ": " << err << std::endl;
    assert(!isnan(err));
    if (err < bestError) {
      bestError = err;
      bestSize = size;
    }
  }
  return bestSize;
}
  
double TrBagg::calcErrorOfSet(unsigned int size, const std::vector<std::vector<Classification> > &classifications) {
  float factor = 1.0 / size;
  double err = 0.0;
  //std::cout << "  -" << std::endl;
  for (int dataInd = targetDataStart; dataInd < (int)data.size(); dataInd++) {
    unsigned int &label = data[dataInd]->label;
    err += 1.0; // will reduce by the amount correct in the loop below
    //double temp = err;
    for (unsigned int classifierInd = 0; classifierInd < size; classifierInd++) {
      err -= factor * classifications[classifierInd][dataInd - targetDataStart][label];
    }
    //std::cout << "  " << *data[dataInd+targetSize] << " -> " << temp - err << std::endl;
    //std::cout << "  " << dataInd << " " << err << std::endl;
  }
  return err;
} 
