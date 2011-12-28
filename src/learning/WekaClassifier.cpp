/*
File: WekaClassifier.cpp
Author: Samuel Barrett
Description: interfaces with weka and moa
Created:  2011-12-26
Modified: 2011-12-26
*/

#include "WekaClassifier.h"
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdio>

const std::string WekaClassifier::WEKA_CMD = "java -DWEKA_HOME=./bin/weka/wekafiles -Xmx2G -cp bin/weka:bin/weka/weka.jar WekaBridge";

WekaClassifier::WekaClassifier(const std::vector<Feature> &features, bool caching, const std::string &opts) :
  Classifier(features,caching)
{
  // get the temp filenames
  char tmp[] = "/tmp/tmpWekaXXXXXX";
  tempDir = mkdtemp(tmp);
  toWekaName = std::string(tempDir + "/to");
  fromWekaName = std::string(tempDir + "/from");

  // make some fifos
  mkfifo(  toWekaName.c_str(),O_CREAT | O_EXCL | S_IRUSR| S_IWUSR); // open it exclusively
  mkfifo(fromWekaName.c_str(),O_CREAT | O_EXCL | S_IRUSR| S_IWUSR); // open it exclusively
  
  // fork you
  pid = fork();
  if (pid < 0) {
    // failed to fork
    std::cerr << "WekaClassifier: ERROR: failed in fork: " << pid << std::endl;
    exit(65);
  } else if (pid == 0) {
    //child
    std::string cmd = WEKA_CMD + " " + toWekaName + " " + fromWekaName + " " + opts;
    char** cmdArr = splitCommand(cmd);
    execvp(cmdArr[0],cmdArr);
    freeCommand(cmdArr);
    //execlp(WEKA_FILE.c_str(),"java","-cp","bin/weka:bin/weka/weka.jar","WekaBridge",toWekaName.c_str(),fromWekaName.c_str(),NULL);
    exit(0);
    return;
  } else { 
    //parent
    //std::cout << "PARENT" << std::endl;
    //std::cout << "c++ opening " << toWekaName << std::endl;
    out.open(toWekaName.c_str());
    //std::cout << "c++ opening " << fromWekaName << std::endl;
    in.open(fromWekaName.c_str());
    //std::cout << "OPENED" << std::endl << std::flush;

    writeFeatures();
    

    //// TODO REMOVE THIS?
    //InstancePtr inst = InstancePtr(new Instance());
    //inst->weight = 1.0;
    //addData(inst);
    //train();
    //out << "save\ntest.weka" << std::endl;
  }
}

WekaClassifier::~WekaClassifier () {
  out.close();
  in.close();
  // kill the other process hopefully
  out << "exit" << std::endl;
  int status;
  wait(&status);
  // clean up the fifos
  remove(tempDir.c_str());
}
  
void WekaClassifier::writeFeatures() {
  for (unsigned int i = 0; i < features.size(); i++) {
    out << features[i].name;
    if (!features[i].numeric) {
      for (unsigned j = 0; j < features[i].values.size(); j++)
        out << "," << features[i].values[j];
    }
    out << std::endl;
  }
  out << "@data" << std::endl;
}

void WekaClassifier::addData(const InstancePtr &instance) {
  out << "add" << std::endl;
  std::cout << "add" << std::endl;
  writeInstance(instance);
}

void WekaClassifier::trainInternal(bool /*incremental*/) {
  out << "train" << std::endl;
  //std::cout << "train" << std::endl;
}

void WekaClassifier::classifyInternal(const InstancePtr &instance, Classification &classification) {
  out << "classify" << std::endl;
  //std::cout << "start classify c++" << std::endl;
  writeInstance(instance);
  classification.clear();
  classification.resize(numClasses);
  float val;
  while (in.peek() == -1) {
    //int i = in.peek();
    //std::cout << i << " " << (i == std::ios_base::eofbit) << " " << (i == std::ios_base::failbit) << std::endl;
    //int j;
    //std::cin >> j;
  }
  const float EPS = 0.001;
  bool unclassified = true;
  for (unsigned int i = 0; i < numClasses; i++) {
    in >> val;
    while (!in.good())
      in >> val;
    if (val > EPS)
      unclassified = false;
    //std::cout << "peek: " << in.peek() << " good: " << in.good() << std::endl;
    //std::cout << "read: " << val << std::endl;
    classification[i] = val;
  }
  if (unclassified) {
    for (unsigned int i = 0; i < numClasses; i++)
      classification[i] = 1.0 / numClasses;
  } 
  //else {
    //std::cout << "classified" << std::endl;
  //}
  //std::cout << "done  classify c++" << std::endl;
}

void WekaClassifier::writeInstance(const InstanceConstPtr &instance) {
  for (unsigned int i = 0; i < features.size(); i++) {
    //std::cout << "out " << i << " " << (*instance)[features[i].name] << std::endl;
    out << instance->get(features[i].name,0) << ",";
  }
  out << instance->weight << std::endl;
  //std::cout << "weight " << instance->weight << std::endl;
  //std::cout << "writeInstance: " << features.size() << std::endl;
}
  
char** WekaClassifier::splitCommand(const std::string &cmd) {
  std::vector<std::string> cmdVec;
  bool escaped = false;
  unsigned int startInd = 0;
  for (unsigned int i = 0; i < cmd.size(); i++) {
    if (escaped) {
      escaped = false;
    } else if (cmd[i] == '\\') {
      escaped = true;
      continue;
    } else if (cmd[i] == ' ') {
      if (startInd != i) { // not just spaces
        cmdVec.push_back(cmd.substr(startInd,i-startInd));
      }
      startInd = i + 1;
    }
  }
  if (startInd != cmd.size())
    cmdVec.push_back(cmd.substr(startInd,cmd.size() - startInd));
  char **cmdArr = new char* [cmdVec.size() + 1]; // + 1 for NULL
  for (unsigned int i = 0; i < cmdVec.size(); i++) {
    cmdArr[i] = new char[cmdVec[i].size()+1];
    for (unsigned int j = 0; j < cmdVec[i].size(); j++)
      cmdArr[i][j] = cmdVec[i][j];
    cmdArr[i][cmdVec[i].size()] = '\0';
    std::cout << "cmdArr[" << i << "]='" << cmdArr[i] << "'" << std::endl;
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
