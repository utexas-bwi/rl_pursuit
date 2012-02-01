#ifndef AGENTPERTURBATION_CP6JBC8K
#define AGENTPERTURBATION_CP6JBC8K

/*
File: AgentPerturbation.h
Author: Samuel Barrett
Description: perturbation of another agent
Created:  2012-02-01
Modified: 2012-02-01
*/

#include "Agent.h"

class AgentPerturbation: public Agent {
public:
  enum PerturbationType {
    NOOP,
    RANDOM
  };

  struct Perturbation {
    PerturbationType type;
    float amount;
  };

  AgentPerturbation(boost::shared_ptr<RNG> rng, const Point2D &dims, const boost::shared_ptr<Agent> &origAgent, const Perturbation &perturbation);

  ActionProbs step(const Observation &obs);
  void restart();
  std::string generateDescription();
  std::string generateLongDescription(unsigned int indentation = 0);

  AgentPerturbation* clone() {
    return new AgentPerturbation(*this);
  }

  void learn(const Observation &prevObs, const Observation &currentObs, unsigned int ind);

protected:
  std::string generatePrefix();

protected:
  boost::shared_ptr<Agent> agent;
  Perturbation perturbation;
};

#endif /* end of include guard: AGENTPERTURBATION_CP6JBC8K */
