#ifndef UTIL_T1FR2WSR
#define UTIL_T1FR2WSR

/*
File: Util.h
Author: Samuel Barrett
Description: a set of utility functions
Created:  2011-08-23
Modified: 2011-08-23
*/

#include <json/json.h>
#include <string>
#include <vector>
#include <ostream>

#ifndef NULL
#define NULL 0
#endif
double getTime();

template <class T>
inline int sgn(const T &x) {
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

template <class T>
inline T min(const T &x1, const T &x2) {
  return x1 < x2 ? x1 : x2;
}

template <class T>
inline T max(const T &x1, const T &x2) {
  return x1 > x2 ? x1 : x2;
}

float softmax(float x1, float x2, float factor); // returns the probability of x1 using a softmax with the given factor

bool readJson(const std::string &filename, Json::Value &value);

std::string indent(unsigned int indentation);

template <class T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &vect) {
  out << "[";
  for (unsigned int i = 0; i < vect.size(); i++) {
    out << vect[i];
    if (i != vect.size() - 1)
      out << ",";
  }
  out << "]";
  return out;
}

#endif /* end of include guard: UTIL_T1FR2WSR */
