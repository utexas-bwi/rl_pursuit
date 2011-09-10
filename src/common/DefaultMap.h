#ifndef DEFAULTMAP_E8UQ8T6
#define DEFAULTMAP_E8UQ8T6

/*
File: DefaultMap.h
Author: Samuel Barrett
Description: a map that returns a default value without inserting when get is called.
Created:  2011-08-23
Modified: 2011-08-23
*/

//#include <map>
#include <boost/unordered_map.hpp>

template <class Key, class T>
class DefaultMap{
public:
  DefaultMap(T defaultValue):
    defaultValue(defaultValue)
  {}

  T get(const Key &key) {
    //typename std::map<Key,T>::iterator it = vals.find(key);
    typename boost::unordered_map<Key,T>::iterator it = vals.find(key);
    if (it == vals.end())
      return defaultValue;
    else
      return it->second;
  }

  T& operator[](const Key &key) {
    if (vals.count(key) == 0)
      vals[key] = defaultValue;
    return vals[key];
  }

  void set(const Key &key, const T &val) {
    vals[key] = val;
  }

  void clear() {
    vals.clear();
  }

  unsigned int size() {
    return vals.size();
  }

private:
  //std::map<Key,T> vals;
  boost::unordered_map<Key,T> vals;
  const T defaultValue;
};

#endif /* end of include guard: DEFAULTMAP_E8UQ8T6 */
