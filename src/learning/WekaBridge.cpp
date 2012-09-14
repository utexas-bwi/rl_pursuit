#include <jni.h>
#include <stdio.h>
#include <iostream>
#include <cassert>
#include "Communicator.h"
#include <boost/shared_ptr.hpp>

boost::shared_ptr<Communicator> comm;
unsigned int NUM_FEATURES;
unsigned int NUM_CLASSES;
bool FIRST_TIME;

extern "C"
JNIEXPORT jint JNICALL Java_WekaBridge_init(JNIEnv *env, jobject , jstring memSegName, jint numFeatures, jint numClasses) {
  const char *str = env->GetStringUTFChars(memSegName, NULL);
  NUM_FEATURES = numFeatures;
  NUM_CLASSES = numClasses;

  assert(str != NULL);
  //std::cout << str << std::endl;
  comm = boost::shared_ptr<Communicator>(new Communicator(str,false,NUM_FEATURES,NUM_FEATURES));
  FIRST_TIME = true;

  //delete[] str;
  env->ReleaseStringUTFChars(memSegName,str);
  return comm->NUM_WEIGHTS;
}

extern "C"
JNIEXPORT jbyte JNICALL Java_WekaBridge_readCommand (JNIEnv *env, jclass , jdoubleArray features, jdoubleArray weight, jdoubleArray weightList) {
  //std::cout << "java waiting" << std::endl;
  comm->wait();

  char cmd = *(comm->cmd);
  //std::cout << "java received " << cmd << std::endl;
  double *arr;
  switch(cmd) {
    case 'r':
      arr = env->GetDoubleArrayElements(weightList,NULL);
      for (unsigned int i = 0; i < comm->NUM_WEIGHTS; i++)
        arr[i] = comm->weightList[i];
      env->ReleaseDoubleArrayElements(weightList, arr, 0);
      break;
    case 'c':
    case 'a':
      arr = env->GetDoubleArrayElements(features,NULL);
      assert(arr != NULL);
      for (unsigned int i = 0; i < NUM_FEATURES; i++)
        arr[i] = comm->features[i];
      env->ReleaseDoubleArrayElements(features, arr, 0);
      arr = env->GetDoubleArrayElements(weight,NULL);
      arr[0] = *(comm->weight);
      env->ReleaseDoubleArrayElements(weight, arr, 0);

      break;
    default:
      // pass
      break;
  }
  //std::cout << "java done reading " << cmd << std::endl;
  return cmd;
}

extern "C"
JNIEXPORT void JNICALL Java_WekaBridge_writeDistr (JNIEnv *env, jclass obj, jdoubleArray distr) {
  double *arr = env->GetDoubleArrayElements(distr,NULL);
  assert(arr != NULL);
  for (unsigned int i = 0; i < NUM_CLASSES; i++)
    comm->classes[i] = arr[i];
  env->ReleaseDoubleArrayElements(distr, arr, 0);
}

extern "C"
JNIEXPORT void JNICALL Java_WekaBridge_send (JNIEnv *env, jclass obj) {
  *(comm->cmd) = '\0';
  comm->send();
}

extern "C"
JNIEXPORT jstring JNICALL Java_WekaBridge_readMsg (JNIEnv *env, jclass obj) {
  return env->NewStringUTF(comm->msg);
}
