#include "NaiveBayes.h"
#include <iostream>

#undef DEBUG_NB

const float NaiveBayes::ALPHA = 0.5;

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
  for (unsigned int i = 0; i < attributes.size(); i++) {
    out << features[i].name << std::endl;
    if (attributes[i].numeric) {
      out << "mean  ";
      for (unsigned int c = 0; c < numClasses; c++)
        out << attributes[i].means[c] << " ";
      out << std::endl;
      out << "stdev  ";
      for (unsigned int c = 0; c < numClasses; c++)
        out << attributes[i].stdevs[c] << " ";
      out << std::endl;
    } else {
      for (unsigned int j = 0; j < attributes[i].probs.size(); j++) {
        out << features[i].values[j] << "  ";
        for (unsigned int c = 0; c < numClasses; c++) {
          out << attributes[i].probs[j][c] << " ";
        }
        out << std::endl;
      }
    }
    out << std::endl;
  }
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
  //for (unsigned int i = 0; i < numClasses; i++)
    //classification[i] = 1.0;
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] = 0.0; // for log

  for (unsigned int i = 0; i < attributes.size(); i++) {
    if (attributes[i].numeric)
      predictContinuousAttribute(instance,i,classification);
    else
      predictDiscreteAttribute(instance,i,classification);
  }

  // subtract the minimum log, to get things in a better frame of referencea
  float maxVal = -std::numeric_limits<float>::infinity();
  for (unsigned int i = 0; i < numClasses; i++)
    if (classification[i] > maxVal)
      maxVal = classification[i];
  //std::cout << "before: ";
  //for (unsigned int i = 0; i < numClasses; i++)
    //std::cout << classification[i] << " ";
  //std::cout << std::endl;
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] -= maxVal;
  //std::cout << "maxVal: " << maxVal << std::endl;
  // convert back from log
  for (unsigned int i = 0; i < numClasses; i++) {
    classification[i] = exp(classification[i]);
    //std::cout << classification[i] << " ";
  }
  //std::cout << std::endl;


  // normalize
  float total = 0;
  for (unsigned int i = 0; i < numClasses; i++)
    total += classification[i];
  for (unsigned int i = 0; i < numClasses; i++)
    classification[i] /= total;
#ifdef DEBUG_NB
  std::cout << "FINAL: ";
  for (unsigned int i = 0; i < numClasses; i++)
    std::cout << classification[i] << " ";
  std::cout << std::endl;
#endif
}

void NaiveBayes::learnDiscreteAttribute(const Feature &feature) {
#ifdef DEBUG_NB
  std::cout << "learn " << feature.name << std::endl;
#endif
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

  // normalize and convert to logs
  for (unsigned int j = 0; j < feature.values.size(); j++) {
    for (unsigned int c = 0; c < numClasses; c++)
      attr.probs[j][c] = log(attr.probs[j][c]) - log(valueWeights[j]);
      //attr.probs[j][c] /= valueWeights[j];
  }

#ifdef DEBUG_NB
  std::cout << "  probs:" << std::endl;
  for (unsigned int j = 0; j < feature.values.size(); j++) {
    std::cout << "  ";
    for (unsigned int c = 0; c < numClasses; c++) {
      std::cout << attr.probs[j][c] << " ";
    }
    std::cout << std::endl;
  }
#endif

  attributes.push_back(attr);
}

void NaiveBayes::learnContinuousAttribute(const Feature &feature) {
#ifdef DEBUG_NB
  std::cout << "learn " << feature.name << std::endl;
#endif
  Attribute attr;
  attr.numeric = true;
  attr.means.resize(numClasses,0);
  attr.stdevs.resize(numClasses,0);
  
  std::vector<float> weights(numClasses,0);
  // get mean of data for each class
  for (unsigned int i = 0; i < data.size(); i++) {
    weights[data[i]->label] += data[i]->weight;
    attr.means[data[i]->label] += data[i]->weight * (*data[i])[feature.name];
  }
  for (unsigned int c = 0; c < numClasses; c++)
    attr.means[c] /= weights[c];
#ifdef DEBUG_NB
  std::cout << "  means: ";
  for (unsigned int c = 0; c < numClasses; c++)
    std::cout << attr.means[c] << " ";
  std::cout << std::endl;
#endif

  // calculate the variance
  float val;
  std::vector<float> counts(numClasses,0);
  for (unsigned int i = 0; i < data.size(); i++) {
    val = (*data[i])[feature.name] - attr.means[data[i]->label];
    attr.stdevs[data[i]->label] += data[i]->weight * val * val;
    counts[data[i]->label] += data[i]->weight;
  }
  // make in stdev instead of variance
  for (unsigned int c = 0; c < numClasses; c++)
    attr.stdevs[c] = sqrt(attr.stdevs[c] / counts[c]);
  // if only one value, settle on 1/6 (arbitrarily, taken from weka)
  const float MIN_VAL = 1.0 / 6.0;
  for (unsigned int c = 0; c < numClasses; c++) {
    if (attr.stdevs[c] < 1e-10)
    //if (attr.stdevs[c] < MIN_VAL)
      attr.stdevs[c] = MIN_VAL;
  }

#ifdef DEBUG_NB
  std::cout << "  stdevs: ";
  for (unsigned int c = 0; c < numClasses; c++)
    std::cout << attr.stdevs[c] << " ";
  std::cout << std::endl;
#endif
  attributes.push_back(attr);
}

void NaiveBayes::predictDiscreteAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c) {
#ifdef DEBUG_NB
  std::cout << "predict discrete " << features[attrInd].name << std::endl;
#endif
  int valueInd = -1;
  for (unsigned int i = 0; i < features[attrInd].values.size(); i++) {
    if (fabs((*instance)[features[attrInd].name] - features[attrInd].values[i]) < 0.01) {
      valueInd = i;
      break;
    }
  }
  assert(valueInd >= 0);
#ifdef DEBUG_NB
  std::cout << "  " << features[attrInd].values.size() << " " << valueInd << std::endl << std::flush;
  std::cout << "  ";
#endif
  for (unsigned int i = 0; i < numClasses; i++) {
    float x = attributes[attrInd].probs[valueInd][i];
    //c[i] *= x;
    c[i] += x; // for log
#ifdef DEBUG_NB
    std::cout << x << " ";
#endif
  }
#ifdef DEBUG_NB
  std::cout << std::endl;
#endif
}

void NaiveBayes::predictContinuousAttribute(const InstancePtr &instance, unsigned int attrInd, Classification &c) {
#ifdef DEBUG_NB
  std::cout << "predict continuous " << features[attrInd].name << std::endl;
  std::cout << "  ";
#endif
  for (unsigned int i = 0; i < numClasses; i++) {
    //float val = ((*instance)[features[attrInd].name] - attributes[attrInd].means[i]);
    //float x = 1 / sqrt(2 * M_PI * attributes[attrInd].vars[i]) * exp(- val * val / (2 * attributes[attrInd].vars[i]));
    float val = (((*instance)[features[attrInd].name] - attributes[attrInd].means[i])) / attributes[attrInd].stdevs[i];
    //float x = 1 / sqrt(2 * M_PI) * exp(-0.5 * val * val);
    //c[i] *= x;
    float x = log(1 / sqrt(2 * M_PI)) - 0.5 * val * val;
    c[i] += x; // for log
#ifdef DEBUG_NB
    std::cout << x << " ";
#endif
  }
#ifdef DEBUG_NB
  std::cout << std::endl;
#endif
}
