#ifndef RNG_6HY8TBAM
#define RNG_6HY8TBAM

/*
File: RNG.h
Author: Samuel Barrett
Description: a random number generator based on tinymt32
Created:  2011-08-23
Modified: 2011-08-23
*/

#include "tinymt32.h"
#include <cstdlib>
#include <vector>
#include <iostream>

class RNG {
public:
  RNG (uint32_t seed) {
    std::cout << "SEED: " << seed << std::endl;
    internal.mat1 = 0x8f7011ee;
    internal.mat2 = 0xfc78ff1f;
    internal.tmat = 0x3793fdff;
    tinymt32::tinymt32_init(&internal,seed);
  }
  float randomFloat() {
    float res = tinymt32::tinymt32_generate_float(&internal);
    std::cout << "RANDOM FLOAT:" << res << std::endl;
    return res;
  }

  uint32_t randomUInt() {
    uint32_t res = tinymt32::tinymt32_generate_uint32(&internal);
    std::cout << "RANDOM UInt:" << res << std::endl;
    return res;
  }

  int32_t randomInt(int32_t max) {
    int32_t res = tinymt32::tinymt32_generate_uint32(&internal) % max;
    std::cout << "RANDOM Int " << max << ": " << res<< std::endl;
    return res;
  }

  int32_t randomInt(int32_t min,int32_t max) {
    uint32_t temp = tinymt32::tinymt32_generate_uint32(&internal);
    int32_t val = temp % (max - min) - min;
    std::cout << "RANDOM Int " << min << " " << max << ": " << val << std::endl;
    return val;
  }
  
  void randomOrdering(std::vector<uint32_t> &inds) {
    uint32_t j;
    uint32_t temp;
    for (uint32_t i = 0; i < inds.size(); i++)
      inds[i] = i;
    for (int i = (int)inds.size()-1; i >= 0; i--) {
      j = randomInt(i+1);
      temp = inds[i];
      inds[i] = inds[j];
      inds[j] = temp;
    }
    std::cout << "RANDOM ORDERING: ";
    //<< std::endl;
    for (unsigned int i = 0; i < inds.size(); i++)
      std::cout << inds[i] << " ";
    std::cout << std::endl;
  }

private:
  tinymt32::tinymt32_t internal;
};

#endif /* end of include guard: RNG_6HY8TBAM */
