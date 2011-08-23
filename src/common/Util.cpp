#include "Util.h"

#include <sys/time.h>

namespace Util {

double getTime() {
  struct timeval time;

  gettimeofday(&time,NULL);
  return time.tv_sec + time.tv_usec / 1000000.0;
}

} //namespace
