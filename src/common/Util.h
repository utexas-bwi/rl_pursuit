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

#ifndef NULL
#define NULL 0
#endif
double getTime();

template <class T>
inline int sgn(const T &x) {
  return (x > 0) ? 1 : ((x < 0) ? -1 : 0);
}

bool readJson(const std::string &filename, Json::Value &value);

std::string indent(unsigned int indentation);

#endif /* end of include guard: UTIL_T1FR2WSR */
