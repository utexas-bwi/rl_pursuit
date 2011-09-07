#include "Util.h"

#include <sys/time.h>
#include <fstream>
#include <cmath>
#include <cassert>

double getTime() {
  struct timeval time;

  gettimeofday(&time,NULL);
  return time.tv_sec + time.tv_usec / 1000000.0;
}

// returns the probability of x1 using a softmax with the given factor
float softmax(float x1, float x2, float factor) {
  x1 = exp(factor * x1);
  x2 = exp(factor * x2);
  return x1 / (x1 + x2);
}

// fills out probs with the probabilities of the vals using a softmax with the given factor
void softmax(const std::vector<unsigned int> &vals, float factor, std::vector<float> &probs) {
  assert(vals.size() >= 1);
  probs.clear();
  std::vector<float> expVals(vals.size());
  float total = 0;

  for (unsigned int i = 0; i < vals.size(); i++) {
    expVals[i] = exp(factor * vals[i]);
    total += expVals[i];
  }
  for (unsigned int i = 0; i < vals.size(); i++) {
    probs.push_back(expVals[i] / total);
  }
}

bool readJson(const std::string &filename, Json::Value &value) {
  Json::Reader reader;
  std::ifstream in(filename.c_str());
  if (!in.good()) {
    std::cerr << "readJson: ERROR opening file: " << filename << std::endl;
    return false;
  }
  Json::Value root;
  bool parsingSuccessful = reader.parse(in,root);
  in.close();
  if (!parsingSuccessful) {
    std::cerr << "readJson: ERROR parsing file: " << filename << std::endl;
    std::cerr << reader.getFormatedErrorMessages();
    return false;
  }
  Json::Value::Members members = root.getMemberNames();
  for (unsigned int i = 0; i < members.size(); i++) {
    value[members[i]] = root[members[i]];
  }

  return true;
}

std::string indent(unsigned int indentation) {
  return std::string(indentation * 2,' ');
}
