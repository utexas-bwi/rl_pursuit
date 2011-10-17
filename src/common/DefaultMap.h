#ifndef DEFAULTMAP_E8UQ8T6
#define DEFAULTMAP_E8UQ8T6

/*
File: DefaultMap.h
Author: Samuel Barrett
Description: a map that returns a default value without inserting when get is called.
Created:  2011-08-23
Modified: 2011-08-23
*/

//#define DEFAULTMAP_USE_BOOST

#ifdef DEFAULTMAP_USE_BOOST
#include <boost/unordered_map.hpp>
#else
#include <map>
#endif

template <class Key, class T>
class DefaultMap{
public:
  DefaultMap(T defaultValue):
    defaultValue(defaultValue)
  {}

  T get(const Key &key) {
#ifdef DEFAULTMAP_USE_BOOST
    typename boost::unordered_map<Key,T>::iterator it = vals.find(key);
#else
    typename std::map<Key,T>::iterator it = vals.find(key);
#endif
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
#ifdef DEFAULTMAP_USE_BOOST
  boost::unordered_map<Key,T> vals;
#else
  std::map<Key,T> vals;
#endif
  const T defaultValue;
};

#endif /* end of include guard: DEFAULTMAP_E8UQ8T6 */
