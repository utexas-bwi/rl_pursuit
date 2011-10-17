#ifndef AGENTDUMMYTEST_WTRLLKOB
#define AGENTDUMMYTEST_WTRLLKOB

/*
File: AgentDummyTest.h
Author: Samuel Barrett
Description: an agent for testing, tracks the number of steps it has taken
Created:  2011-10-17
Modified: 2011-10-17
*/

#include <controller/AgentDummy.h>

class AgentDummyTest: public AgentDummy {
public:
  AgentDummyTest(boost::shared_ptr<RNG> rng, const Point2D &dims):
    AgentDummy(rng,dims),
    numSteps(0)
  {}

  ActionProbs step(const Observation &obs) {
    numSteps++;
    return AgentDummy::step(obs);
  }
  unsigned int numSteps;
};

#endif /* end of include guard: AGENTDUMMYTEST_WTRLLKOB */
