#include "Util.h"

#include <sys/time.h>
#include <fstream>

double getTime() {
  struct timeval time;

  gettimeofday(&time,NULL);
  return time.tv_sec + time.tv_usec / 1000000.0;
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
