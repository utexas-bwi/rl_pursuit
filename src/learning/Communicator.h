#ifndef COMMUNICATOR_5AAH7E5U
#define COMMUNICATOR_5AAH7E5U

/*
File: Communicator.h
Author: Samuel Barrett
Description: communicates using shared memory
Created:  2011-12-28
Modified: 2011-12-28
*/

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/interprocess_condition.hpp>

class Communicator {
public:
  Communicator(const std::string &memoryName, bool removeOnExit, unsigned int numFeatures, unsigned int numClasses);
  ~Communicator();
  
  void send();
  void wait();
  void sendWait(char ch);

protected:
  const std::string memoryName;
  static const unsigned int MEMORY_SIZE;
  boost::interprocess::managed_shared_memory *memSegment;
  bool removeOnExit;
  boost::interprocess::interprocess_condition *cond;
  unsigned int numFeatures;
  unsigned int numClasses;

public:
  static const unsigned int MSG_SIZE;
  static const unsigned int NUM_INSTANCES;
  double *features;
  double *classes;
  char *cmd;
  double *weight;
  char *msg;
  int *n;
};

#endif /* end of include guard: COMMUNICATOR_5AAH7E5U */
