/*
File: WekaClassifier.cpp
Author: Samuel Barrett
Description: interfaces with weka and moa
Created:  2011-12-26
Modified: 2011-12-26
*/

#include "WekaClassifier.h"
#include "Communicator.h"
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <boost/lexical_cast.hpp>
#include <common/Util.h>

#if COMPILE_ARCH == 64
const std::string WekaClassifier::ARCH_DEP_CMD = "64 -Xmx4G";
#elif COMPILE_ARCH == 32
const std::string WekaClassifier::ARCH_DEP_CMD = "32 -Xmx2G";
#else
#error "COMPILE_ARCH must either be 64 or 32"
#endif

const std::string WekaClassifier::WEKA_CMD = std::string("java -DWEKA_HOME=./bin/weka/wekafiles -Djava.library.path=bin/") + ARCH_DEP_CMD + " -cp bin/weka:bin/weka/weka.jar WekaBridge";
int WekaClassifier::classifierCount = 0;

WekaClassifier::WekaClassifier(const std::vector<Feature> &features, bool caching, const std::string &opts) :
  Classifier(features,caching),
  dropFrac(0.0),
  trainAllowed(true)
{
  classifierCount++;
  memSegName = "WEKA_BRIDGE_" + boost::lexical_cast<std::string>(getpid()) + "_" + boost::lexical_cast<std::string>(classifierCount);
  comm = boost::shared_ptr<Communicator>(new Communicator(memSegName,true,features.size(),numClasses));
  // fork you
  pid = fork();
  if (pid < 0) {
    // failed to fork
    std::cerr << "WekaClassifier: ERROR: failed in fork: " << pid << std::endl;
    exit(65);
  } else if (pid == 0) {
    //child
    std::string cmd = WEKA_CMD + " " + memSegName + " " + "data/dt/blank.arff" + " " + opts;
    char** cmdArr = splitCommand(cmd);
    execvp(cmdArr[0],cmdArr);
    freeCommand(cmdArr);
    exit(0);
    return;
  } else { 
    //parent
  }
  *(comm->cmd) = '\0';
}

WekaClassifier::~WekaClassifier () {
  //std::cout << "DESTRUCTOR" << std::endl;
  if (comm.unique()) {
    //std::cout << "Stop Weka" << std::endl;
    // kill the other process hopefully
    *(comm->cmd) = 'e';
    comm->send();
    int status;
    wait(&status);
  }
}

void WekaClassifier::sendInstances(bool force) {
  int &n = *(comm->n);
  if ((n > 0) && (force || (n == (int)comm->NUM_INSTANCES))) {
    //std::cout << "SENDING" << std::endl;
    comm->sendWait('a');
    *(comm->n) = 0;
  }
}
  
void WekaClassifier::addData(const InstancePtr &instance) {
  if ((dropFrac > 1e-10) && (rng->randomFloat() < dropFrac))
    return;
  //std::cout << "adding " << *instance << std::endl;
  writeInstance(instance);
  sendInstances(false);

  //strcpy(comm->msg,"TESTING THIS OUT");
  //*(comm->cmd) = 'w';
  //comm->send();
  //comm->wait();
}
  
void WekaClassifier::save(const std::string &filename) const {
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->sendWait('s');
}

void WekaClassifier::saveAsOutput(const std::string &filename) const {
  // convert to DT
  std::string tempfile = tempFilename();
  outputDescriptionToFile(tempfile);
  int numInitialLinesToRemove = 3;
  std::ifstream in(tempfile.c_str());
  std::ofstream out(filename.c_str());
  std::string line;
  std::getline(in,line);
  outputHeader(out);
  out << std::endl;
  while (in.good()) {
    if (numInitialLinesToRemove > 0)
      numInitialLinesToRemove--;
    else
      out << line << std::endl;
    std::getline(in,line);
  }
  in.close();
  out.close();
  remove(tempfile.c_str());
}

bool WekaClassifier::load(const std::string &filename) {
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->sendWait('l');
  return true;
}
  
void WekaClassifier::outputDescriptionToFile(const std::string &filename) const {
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->sendWait('w');
}

void WekaClassifier::clearData() {
  comm->sendWait('x');
}

void WekaClassifier::trainInternal(bool /*incremental*/) {
  assert(trainAllowed);
  sendInstances(true);
  //std::cout << "train" << std::endl;
  comm->sendWait('t');
}

void WekaClassifier::classifyInternal(const InstancePtr &instance, Classification &classification) {
  sendInstances(true);
  //std::cout << "classifying" << std::endl;
  writeInstance(instance);
  comm->sendWait('c');
  *(comm->n) = 0;
  classification.resize(numClasses);
  
  const float EPS = 0.001;
  bool unclassified = true;
  for (unsigned int i = 0; i < numClasses; i++) {
    classification[i] = comm->classes[i];
    if (classification[i] > EPS)
      unclassified = false;
  }
  if (unclassified) {
    for (unsigned int i = 0; i < numClasses; i++)
      classification[i] = 1.0 / numClasses;
  } 
}

void WekaClassifier::writeInstance(const InstanceConstPtr &instance) {
  int &n = *(comm->n);
  assert(n < (int)comm->NUM_INSTANCES);
  int offset = n * features.size();
  for (unsigned int i = 0; i < features.size(); i++) {
    comm->features[offset + i] = instance->get(features[i].feat,0);
  }
  comm->weight[n] = instance->weight;
  n++;
}
  
char** WekaClassifier::splitCommand(const std::string &cmd) {
  std::vector<std::string> cmdVec;
  //std::cout << "splitting cmd: " << cmd << std::endl;
  //bool escaped = false;
  bool quoted = false;
  //unsigned int startInd = 0;
  std::string str;
  for (unsigned int i = 0; i < cmd.size(); i++) {
    //if (escaped) {
      //escaped = false;
    //} else if (cmd[i] == '\\') {
      //escaped = true;
      //continue;
    //} else if (cmd[i] == '"') {
      //quoted = !quoted;
    //} else if ((cmd[i] == ' ') && !quoted) {
      //if (!str.empty()) { // not just spaces
        //cmdVec.push_back(str);
      //}
      //str.clear();
    //}
    if (cmd[i] == '"') {
      quoted = !quoted;
    } else if ((cmd[i] == ' ') && !quoted) {
      if (!str.empty()) { // not just spaces
        cmdVec.push_back(str);
      }
      str.clear();
    } else {
      str += cmd[i];
    }
  }
  //if (startInd != cmd.size())
    //cmdVec.push_back(cmd.substr(startInd,cmd.size() - startInd));
  if (!str.empty())
    cmdVec.push_back(str);
  char **cmdArr = new char* [cmdVec.size() + 1]; // + 1 for NULL
  for (unsigned int i = 0; i < cmdVec.size(); i++) {
    cmdArr[i] = new char[cmdVec[i].size()+1];
    for (unsigned int j = 0; j < cmdVec[i].size(); j++)
      cmdArr[i][j] = cmdVec[i][j];
    cmdArr[i][cmdVec[i].size()] = '\0';
  }
  cmdArr[cmdVec.size()] = NULL;
  return cmdArr;
}

void WekaClassifier::freeCommand(char **cmdArr) {
  int i = 0;
  while (cmdArr[i] != NULL) {
    delete[] cmdArr[i];
    i++;
  }
}

void WekaClassifier::outputDescription(std::ostream &out) const {
  outputHeader(out);
  out << std::endl;
  comm->sendWait('p');
}
  
WekaClassifier* WekaClassifier::copyWithWeights(const InstanceSet &data) {
  trainAllowed = false;
  WekaClassifier *c = new WekaClassifier(*this);
  c->trainAllowed = true; // need this in case we copy from one where it's not allowed
  int &ind = *(comm->n);
  assert(ind == 0);
  for (unsigned int i = 0; i < data.size(); i++) {
    comm->weight[ind] = data[i]->weight;
    ind++;
    if (ind == (int)comm->NUM_INSTANCES) {
      //std::cout << "sending full weights" << std::endl;
      comm->sendWait('r');
      ind = 0;
    }
  }
  if (ind != 0) {
    comm->sendWait('r');
    ind = 0;
  }
  return c;
}
