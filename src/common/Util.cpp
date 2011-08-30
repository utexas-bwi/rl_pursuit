#include "Util.h"

#include <sys/time.h>
#include <fstream>
#include <cmath>

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

bool readJson(const std::string &filename, Json::Value &value) {
  Json::Reader reader;
  std::ifstream in(filename.c_str());
  if (!in.good()) {
    std::cerr << "readJson: ERROR opening file: " << filename << std::endl;
    return false;
  }
  bool parsingSuccessful = reader.parse(in,value);
  in.close();
  if (!parsingSuccessful) {
    std::cerr << "readJson: ERROR parsing file: " << filename << std::endl;
    std::cerr << reader.getFormatedErrorMessages();
    return false;
  }
  return true;
}

std::string indent(unsigned int indentation) {
  return std::string(indentation * 2,' ');
}
