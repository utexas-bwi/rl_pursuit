/*
File: AgentPerturbation.cpp
Author: Samuel Barrett
Description: perturbation of another agent
Created:  2012-02-01
Modified: 2012-02-01
*/

#include "AgentPerturbation.h"
#include <boost/lexical_cast.hpp>

AgentPerturbation::AgentPerturbation(boost::shared_ptr<RNG> rng, const Point2D &dims, const boost::shared_ptr<Agent> &origAgent, const Perturbation &perturbation):
  Agent(rng,dims),
  agent(origAgent),
  perturbation(perturbation)
{
}

ActionProbs AgentPerturbation::step(const Observation &obs) {
  ActionProbs action = agent->step(obs);
  //std::cout << "pre perturbation: " << action << std::endl;;

  ActionProbs pertAction;
  switch (perturbation.type) {
    case NOOP:
      pertAction[Action::NOOP] = 1.0;
      break;
    case RANDOM:
      for (unsigned int a = 0; a < Action::NUM_ACTIONS; a++) 
        pertAction[(Action::Type)a] = 1.0 / Action::NUM_ACTIONS;
      break;
  }
  
  for (unsigned int a = 0; a < Action::NUM_ACTIONS; a++) {
    Action::Type act = (Action::Type)a;
    action[act] = (1.0 - perturbation.amount) * action[act] + perturbation.amount * pertAction[act];
  }
  
  //std::cout << "post perturbation: " << action << std::endl;;
  return action;
}

void AgentPerturbation::restart() {
  agent->restart();
}

std::string AgentPerturbation::generateDescription() {
  return generatePrefix() + " " + agent->generateDescription();
}

std::string AgentPerturbation::generateLongDescription(unsigned int indentation) {
  return indent(indentation) + generatePrefix() + "\n" + agent->generateLongDescription(indentation + 1);
}

void AgentPerturbation::learn(const Observation &prevObs, const Observation &currentObs, unsigned int ind) {
  agent->learn(prevObs,currentObs,ind);
}

std::string AgentPerturbation::generatePrefix() {
  std::string msg = "AgentPerturbation: ";
  switch (perturbation.type) {
    case NOOP:
      msg += "NOOP";
      break;
    case RANDOM:
      msg += "RAND";
      break;
  }
  msg += "(" + boost::lexical_cast<std::string>(perturbation.amount) + ")";
  return msg;
}
