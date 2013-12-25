#include "Communicator.h"
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/thread/thread_time.hpp>

const unsigned int Communicator::MSG_SIZE = 1000;
const unsigned int Communicator::NUM_INSTANCES = 10000;
const unsigned int Communicator::MEMORY_SIZE = MSG_SIZE + NUM_INSTANCES * (21 * sizeof(double)) + 20;

Communicator::Communicator(const std::string &memoryName, bool removeOnExit, unsigned int numFeatures, unsigned int numClasses):
  removeOnExit(removeOnExit)
{
  if (removeOnExit)
    boost::interprocess::shared_memory_object::remove(memoryName.c_str());

  memSegment = new boost::interprocess::managed_shared_memory(boost::interprocess::open_or_create,memoryName.c_str(),MEMORY_SIZE);
  cond = memSegment->find_or_construct<boost::interprocess::interprocess_condition>("cond")();
  features = memSegment->find_or_construct<double>("features")[numFeatures * NUM_INSTANCES]();
  classes = memSegment->find_or_construct<double>("classes")[numClasses]();
  cmd = memSegment->find_or_construct<char>("command")();
  weight = memSegment->find_or_construct<double>("weight")[NUM_INSTANCES]();
  msg = memSegment->find_or_construct<char>("msg")[MSG_SIZE]();
  n = memSegment->find_or_construct<int>("n")();
}

Communicator::~Communicator() {
  if (removeOnExit)
    boost::interprocess::shared_memory_object::remove(memoryName.c_str());
}

void Communicator::send() {
  cond->notify_one();
}
//#include <iostream>
void Communicator::wait() {
  // crap that's required for wait
  boost::interprocess::interprocess_mutex mutex;
  boost::interprocess::scoped_lock<boost::interprocess::interprocess_mutex> lock(mutex);
  // actually wait
  while (true) {
    if (removeOnExit) {
      if (*cmd == '\0')
        return;
      //std::cout << "removeOnExit cmd == " << (int)*cmd << std::endl;
    } else {
      if (*cmd != '\0')
        return;
      //std::cout << "no removeOnExit cmd == " << (int)*cmd << std::endl;
    }
    boost::system_time t = boost::get_system_time() + boost::posix_time::milliseconds(1);
    cond->timed_wait(lock,t);
    //cond->wait(lock);
  }
}

void Communicator::sendWait(char ch) {
  *cmd = ch;
  send();
  wait();
}
