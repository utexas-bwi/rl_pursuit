#include "Util.h"

#include <sys/time.h>
#include <fstream>
#include <cmath>
#include <cassert>
#include <gflags/gflags.h>
#include <stdlib.h>
#include <limits>

double getTime() {
  struct timeval time;

  gettimeofday(&time,NULL);
  return time.tv_sec + time.tv_usec / 1000000.0;
}

std::map<int,double> TIME_MAP;

void tic(int id) {
  TIME_MAP[id] = getTime();
}

double toc(int id) {
  return getTime() - TIME_MAP[id];
}

void toc(double &counter, int id) {
  counter += toc(id);
}

unsigned int vectorMaxInd(const std::vector<float> &arr) {
  float maxVal;
  unsigned int maxInd = 0;
  vectorMax(arr,maxVal,maxInd);
  return maxInd;
}


void vectorMax(const std::vector<float> &arr, float &maxVal, unsigned int &maxInd) {
  maxVal = -1 * std::numeric_limits<float>::infinity();
  for (unsigned int i = 0; i < arr.size(); i++) {
    if (arr[i] > maxVal) {
      maxVal = arr[i];
      maxInd = i;
    }
  }
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

struct ReplaceMap {
  std::map<std::string,std::string> replacementMap;

  ReplaceMap(const std::map<std::string,std::string> &replacementMap):
    replacementMap(replacementMap)
  {
  }

  void operator() (Json::Value &value) {
    // change this value if necessary
    if (value.isString()) {
      std::map<std::string,std::string>::const_iterator it;
      std::string str = value.asString();
      for (it = replacementMap.begin(); it != replacementMap.end(); it++) {
        size_t pos = str.find(it->first);
        if (pos != std::string::npos) {
          str.replace(pos,it->first.length(),it->second);
        }
      }
      value = Json::Value(str);
    }
  }
};

void jsonReplaceStrings(Json::Value &value, const std::map<std::string,std::string> &replacementMap) {
  jsonReplaceStrings(value,ReplaceMap(replacementMap));
}

void jsonReplaceStrings(Json::Value &value, boost::function<void (Json::Value &)> replace) {
  replace(value);
  // check its children
  Json::Value::iterator child;
  if (value.isObject()) {
    std::vector<std::string> names = value.getMemberNames();
    for (unsigned int i = 0; i < names.size(); i++) {
      jsonReplaceStrings(value[names[i]],replace);
    }
  } else if (value.isArray()) {
    for (unsigned int i = 0; i < value.size(); i++) {
      jsonReplaceStrings(value[i],replace);
    }
  }
}

std::string indent(unsigned int indentation) {
  return std::string(indentation * 2,' ');
}

DEFINE_bool(h,false,"Print help");

void parseCommandLineArgs(int *argc, char ***argv, const std::string &usage, int minArgs, int maxArgs) {
  google::SetUsageMessage(usage);
  google::ParseCommandLineNonHelpFlags(argc, argv, true);
  
  // check if help was set
  std::string helpStr;
  google::GetCommandLineOption("help",&helpStr);
  if ((helpStr == "true") || FLAGS_h)
    printCommandLineHelpAndExit();

  google::HandleCommandLineHelpFlags();
  // check the number of remaining arguments
  if (((minArgs >= 0) && (*argc -1 < minArgs)) || ((maxArgs >= 0) && (*argc - 1 > maxArgs))) {
    std::cerr << "WARNING: Incorrect number of arguments, got " << *argc - 1 << " expected ";
    if (maxArgs == minArgs)
      std::cerr << minArgs;
    else
      std::cerr << minArgs << "-" << maxArgs;
    std::cerr << std::endl;
    printCommandLineHelpAndExit();
  }
}

void printCommandLineHelpAndExit() {
  std::cout << google::GetArgv0() << ": " << google::ProgramUsage() << std::endl;
  std::vector<google::CommandLineFlagInfo> flags;
  google::GetAllFlags(&flags);
  for (unsigned int i = 0; i < flags.size(); i++) {
    if (((flags[i].filename == "src/gflags.cc") || (flags[i].filename == "src/gflags_completions.cc") || (flags[i].filename == "src/gflags_reporting.cc")) && (flags[i].name != "help") && (flags[i].name != "helpfull"))
      continue;
    std::cout << google::DescribeOneFlag(flags[i]);
  }
  exit(1);
}
