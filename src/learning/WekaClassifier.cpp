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

const std::string WekaClassifier::WEKA_CMD = std::string("java -DWEKA_HOME=./bin/weka/wekafiles -Djava.library.path=bin/") + COMPILE_ARCH + " -Xmx2G -cp bin/weka:bin/weka/weka.jar WekaBridge";
int WekaClassifier::classifierCount = 0;

WekaClassifier::WekaClassifier(const std::vector<Feature> &features, bool caching, const std::string &opts) :
  Classifier(features,caching),
  dropFrac(0.0)
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
  // kill the other process hopefully
  *(comm->cmd) = 'e';
  comm->send();
  int status;
  wait(&status);
}
  
void WekaClassifier::addData(const InstancePtr &instance) {
  if ((dropFrac > 1e-10) && (rng->randomFloat() < dropFrac))
    return;
  //std::cout << "adding " << *instance << std::endl;
  writeInstance(instance);
  *(comm->cmd) = 'a';
  comm->send();
  comm->wait();

  //strcpy(comm->msg,"TESTING THIS OUT");
  //*(comm->cmd) = 'w';
  //comm->send();
  //comm->wait();
}
  
void WekaClassifier::save(const std::string &filename) const {
  *(comm->cmd) = 's';
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->send();
  comm->wait();
}

void WekaClassifier::saveAsOutput(const std::string &filename) const {
  // convert to DT
  std::string tempfile = tmpnam(NULL);
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
  *(comm->cmd) = 'l';
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->send();
  comm->wait();
  return true;
}
  
void WekaClassifier::outputDescriptionToFile(const std::string &filename) const {
  *(comm->cmd) = 'w';
  strncpy(comm->msg,filename.c_str(),comm->MSG_SIZE-2);
  comm->msg[comm->MSG_SIZE-1] = '\0';
  comm->send();
  comm->wait();
}

void WekaClassifier::clearData() {
  *(comm->cmd) = 'x';
  comm->send();
  comm->wait();
}

void WekaClassifier::trainInternal(bool /*incremental*/) {
  //std::cout << "train" << std::endl;
  *(comm->cmd) = 't';
  comm->send();
  comm->wait();
}

void WekaClassifier::classifyInternal(const InstancePtr &instance, Classification &classification) {
  //std::cout << "classifying" << std::endl;
  writeInstance(instance);
  *(comm->cmd) = 'c';
  comm->send();
  //std::cout << "waiting" << std::endl;
  comm->wait();
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
  for (unsigned int i = 0; i < features.size(); i++) {
    comm->features[i] = instance->get(features[i].feat,0);
    //std::cout << i << " " << features[i].name << " " << comm->features[i] << std::endl;
  }
  *(comm->weight) = instance->weight;
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
  *(comm->cmd) = 'p';
  comm->send();
  comm->wait();
}
