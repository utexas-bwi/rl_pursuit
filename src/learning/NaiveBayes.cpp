#include "NaiveBayes.h"
#include <iostream>

const float NaiveBayes::ALPHA = 0.1;

NaiveBayes::NaiveBayes(const std::vector<Feature> &features, bool caching):
  Classifier(features,caching),
  data(numClasses)
{
}

NaiveBayes::~NaiveBayes() {
}

void NaiveBayes::addData(const InstancePtr &instance) {
  data.add(instance);
}

void NaiveBayes::outputDescription(std::ostream &out) const {
  out << "Naive Bayes" << std::endl;
}

void NaiveBayes::trainInternal(bool incremental) {
  assert(!incremental);
  attributes.clear();
  for (unsigned int featureInd = 0; featureInd < features.size() - 1; featureInd++) { /// -1 to skip the true class
    if (features[featureInd].numeric)
      learnContinuousAttribute(features[featureInd]);
    else
      learnDiscreteAttribute(features[featureInd]);
  }
  assert(features.size() - 1 == attributes.size());
}

void NaiveBayes::classifyInternal(const InstancePtr &instance, Classification &classification) {
  // start with all probs 1.0
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] = 1.0;

  for (unsigned int i = 0; i < features.size() - 1; i++) { // still skipping the last attribute
    if (attributes[i].numeric)
      predictContinuousAttribute(instance,i,classification);
    else
      predictDiscreteAttribute(instance,i,classification);
  }
  // normalize
  float total = 0;
  for (unsigned int i = 0; i < numClasses; i++)
    total += classification[i];
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] /= total;

  std::cout << "FINAL: ";
  for (unsigned int i = 0; i < numClasses; i++)
    std::cout << classification[i] << " ";
  std::cout << std::endl;
}

void NaiveBayes::learnDiscreteAttribute(const Feature &feature) {
  std::cout << "learn " << feature.name << std::endl;
  Attribute attr;
  // just calculate the fraction with each val 
  attr.numeric = false;

  attr.probs.resize(feature.values.size(),std::vector<float>(numClasses,0));

  std::vector<float> valueWeights(feature.values.size(),0);
  for (unsigned int i = 0; i < data.size(); i++) {
    for (unsigned int j = 0; j < feature.values.size(); j++) {
      if (fabs((*data[i])[feature.name] - feature.values[j]) < 0.01) {
        valueWeights[j] += data[i]->weight; // increment tho total weight
        attr.probs[j][data[i]->label] += data[i]->weight; // increment the weight of the correct class
        break;
      }
    }
  }
  
  // handle unseen values
  for (unsigned int j = 0; j < feature.values.size(); j++) {
    valueWeights[j] += numClasses * ALPHA;
    for (unsigned int c = 0; c < numClasses; c++) {
      attr.probs[j][c] += ALPHA;
    }
  }

  // normalize
  for (unsigned int j = 0; j < feature.values.size(); j++) {
    for (unsigned int c = 0; c < numClasses; c++)
      attr.probs[j][c] /= valueWeights[j];
  }

  std::cout << "  probs:" << std::endl;
  for (unsigned int j = 0; j < feature.values.size(); j++) {
    std::cout << "  ";
    for (unsigned int c = 0; c < numClasses; c++) {
      std::cout << attr.probs[j][c] << " ";
    }
    std::cout << std::endl;
  }

  attributes.push_back(attr);
}

void NaiveBayes::learnContinuousAttribute(const Feature &feature) {
  std::cout << "learn " << feature.name << std::endl;
  Attribute attr;
  attr.numeric = true;
  attr.means.resize(numClasses,0);
  attr.vars.resize(numClasses,0);
  
  std::vector<float> weights(numClasses,0);
  // get mean of data for each class
  for (unsigned int i = 0; i < data.size(); i++) {
    weights[data[i]->label] += data[i]->weight;
    attr.means[data[i]->label] += data[i]->weight * (*data[i])[feature.name];
  }
  for (unsigned int c = 0; c < numClasses; c++)
    attr.means[c] /= weights[c];
  std::cout << "  means: ";
  for (unsigned int c = 0; c < numClasses; c++)
    std::cout << attr.means[c] << " ";
  std::cout << std::endl;

  // calculate the variance
  float val;
  for (unsigned int i = 0; i < data.size(); i++) {
    val = (*data[i])[feature.name] - attr.means[data[i]->label];
    attr.vars[data[i]->label] += data[i]->weight * val * val;
  }
  //for (unsigned int c = 0; c < numClasses; c++)
    //attr.vars[c] += ALPHA;

  for (unsigned int c = 0; c < numClasses; c++) {
    if (attr.vars[c] < 0.01)
      attr.vars[c] = 0.01;
  }
  std::cout << "  vars: ";
  for (unsigned int c = 0; c < numClasses; c++)
    std::cout << attr.vars[c] << " ";
  std::cout << std::endl;
  attributes.push_back(attr);
}

void NaiveBayes::predictDiscreteAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c) {
  std::cout << "predict discrete " << features[attrInd].name << std::endl;
  int valueInd = -1;
  for (unsigned int i = 0; i < features[attrInd].values.size(); i++) {
    if (fabs((*instance)[features[attrInd].name] - features[attrInd].values[i]) < 0.01) {
      valueInd = i;
      break;
    }
  }
  assert(valueInd >= 0);
  std::cout << "  " << features[attrInd].values.size() << " " << valueInd << std::endl << std::flush;
  std::cout << "  ";
  for (unsigned int i = 0; i < numClasses; i++) {
    float x = attributes[attrInd].probs[valueInd][i];
    c[i] *= x;
    std::cout << x << " ";
  }
  std::cout << std::endl;
}

void NaiveBayes::predictContinuousAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c) {
  std::cout << "predict continuous " << features[attrInd].name << std::endl;
  std::cout << "  ";
  for (unsigned int i = 0; i < numClasses; i++) {
    //float val = ((*instance)[features[attrInd].name] - attributes[attrInd].means[i]);
    //float x = 1 / sqrt(2 * M_PI * attributes[attrInd].vars[i]) * exp(- val * val / (2 * attributes[attrInd].vars[i]));
    float val = (((*instance)[features[attrInd].name] - attributes[attrInd].means[i])) / sqrt(attributes[attrInd].vars[i]);
    float x = 1 / sqrt(2 * M_PI) * exp(-0.5 * val * val);
    c[i] *= x;
    std::cout << x << " ";
  }
  std::cout << std::endl;
}
