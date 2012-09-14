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
  comm = boost::shared_ptr<Communicator>(new Communicator(str,false,NUM_FEATURES,NUM_CLASSES));
  FIRST_TIME = true;

  //delete[] str;
  env->ReleaseStringUTFChars(memSegName,str);
  return comm->NUM_INSTANCES;
}

extern "C"
JNIEXPORT jbyte JNICALL Java_WekaBridge_readCommand (JNIEnv *env, jclass , jobjectArray features, jdoubleArray weight, jintArray nArr) {
  //std::cout << "java waiting" << std::endl;
  comm->wait();

  char cmd = *(comm->cmd);
  int &n = *(comm->n);
  env->SetIntArrayRegion(nArr,0,1,comm->n);
  //std::cout << "java received " << cmd << std::endl;
  //double *arr;
  switch(cmd) {
    case 'r':
      env->SetDoubleArrayRegion(weight,0,n,comm->weight);
      //arr = env->GetDoubleArrayElements(weight,NULL);
      //for (int i = 0; i < n; i++)
        //arr[i] = comm->weight[i];
      //env->ReleaseDoubleArrayElements(weight, arr, 0);
      break;
    case 'c':
    case 'a':
      //env->SetDoubleArrayRegion(features);
      //arr = env->GetDoubleArrayElements(features,NULL);
      //assert(arr != NULL);
      //for (unsigned int i = 0; i < NUM_FEATURES * n; i++)
        //arr[i] = comm->features[i];
      //env->ReleaseDoubleArrayElements(features, arr, 0);
      env->SetDoubleArrayRegion(weight,0,n,comm->weight);
      for (int i = 0; i < n; i++) {
        jdoubleArray arr = (jdoubleArray)(env->GetObjectArrayElement(features,i));
        env->SetDoubleArrayRegion(arr,0,NUM_FEATURES,&(comm->features[i*NUM_FEATURES]));
      }
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
  env->GetDoubleArrayRegion(distr,0,NUM_CLASSES,comm->classes);
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
