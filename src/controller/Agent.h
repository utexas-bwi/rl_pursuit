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
#include <common/Util.h>

class Agent {
public:
  Agent(boost::shared_ptr<RNG> rng, const Point2D &dims);

  virtual ~Agent();

  virtual ActionProbs step(const Observation &obs) = 0;
  virtual void restart() = 0; // between episodes
  virtual std::string generateDescription() = 0;
  virtual std::string generateLongDescription(unsigned int indentation = 0);
  virtual Agent* clone() = 0;
  virtual void learn(const Observation &prevObs, const Observation &currentObs, unsigned int ind);

protected:
  boost::shared_ptr<RNG> rng;
  const Point2D dims;
};

#endif /* end of include guard: AGENT_BDMSDS5P */
