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

const std::string WekaClassifier::WEKA_FILE = "java";
const std::string WekaClassifier::WEKA_CMD = "java -cp bin/weka WekaBridge";

WekaClassifier::WekaClassifier(const std::vector<Feature> &features, std::string /*wekaOptions*/) :
  Classifier(features)
{
  // get the temp filenames
  char tmp[] = "/tmp/tmpWekaXXXXXX";
  tempDir = mkdtemp(tmp);
  toWekaName = std::string(tempDir + "/to");
  fromWekaName = std::string(tempDir + "/from");

  // make some fifos
  mkfifo(  toWekaName.c_str(),O_CREAT | O_EXCL | S_IRUSR| S_IWUSR); // open it exclusively
  mkfifo(fromWekaName.c_str(),O_CREAT | O_EXCL | S_IRUSR| S_IWUSR); // open it exclusively
  std::cout << "DONE CREATING FIFOS" << std::endl;
  
  // fork you
  pid = fork();
  if (pid < 0) {
    // failed to fork
    std::cerr << "WekaClassifier: ERROR: failed in fork: " << pid << std::endl;
    exit(65);
  } else if (pid == 0) {
    //child
    //std::string cmd = WEKA_CMD + " " + toWekaName + " " + fromWekaName + " " + wekaOptions;
    //std::cout << cmd << std::endl;
    //std::cout << "child" << std::endl << std::flush;
    execlp(WEKA_FILE.c_str(),"java","-cp","bin/weka:bin/weka/weka.jar","WekaBridge",toWekaName.c_str(),fromWekaName.c_str(),NULL);
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

    InstancePtr inst = InstancePtr(new Instance());
    inst->weight = 1.0;
    addData(inst);
    train();
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

void WekaClassifier::train(bool /*incremental*/) {
  out << "train" << std::endl;
  std::cout << "train" << std::endl;
}

void WekaClassifier::classify(const InstancePtr &instance, Classification &classification) {
  out << "classify" << std::endl;
  //std::cout << "start classify c++" << std::endl;
  writeInstance(instance);
  classification.clear();
  float val;
  while (in.peek() == -1) {
    //int i = in.peek();
    //std::cout << i << " " << (i == std::ios_base::eofbit) << " " << (i == std::ios_base::failbit) << std::endl;
    //int j;
    //std::cin >> j;
  }
  for (unsigned int i = 0; i < numClasses; i++) {
    in >> val;
    while (!in.good())
      in >> val;
    //std::cout << "peek: " << in.peek() << " good: " << in.good() << std::endl;
    //std::cout << "read: " << val << std::endl;
    classification.push_back(val);
  }
  //std::cout << "done  classify c++" << std::endl;
}

void WekaClassifier::writeInstance(const InstancePtr &instance) {
  for (unsigned int i = 0; i < features.size(); i++) {
    //std::cout << "out " << i << " " << (*instance)[features[i].name] << std::endl;
    out << (*instance)[features[i].name] << ",";
  }
  out << instance->weight << std::endl;
  //std::cout << "weight " << instance->weight << std::endl;
  //std::cout << "writeInstance: " << features.size() << std::endl;
}
