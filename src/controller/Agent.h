#ifndef AGENT_BDMSDS5P
#define AGENT_BDMSDS5P

/*
File: Agent.h
Author: Samuel Barrett
Description: Abstract agent type
Created:  2011-08-22
Modified: 2011-08-22
*/

#include <string>
#include <boost/shared_ptr.hpp>

#include <common/RNG.h>
#include <model/Common.h>

class Agent {
public:
  Agent(boost::shared_ptr<RNG> rng, const Point2D &dims):
    rng(rng),
    dims(dims)
  {}

  virtual Action::Type step(const Observation &obs) = 0;
  virtual void restart() = 0; // between episodes
  virtual std::string generateDescription() = 0;

protected:
  boost::shared_ptr<RNG> rng;
  const Point2D dims;
};

#endif /* end of include guard: AGENT_BDMSDS5P */
