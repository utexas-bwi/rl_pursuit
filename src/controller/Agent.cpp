#include "Agent.h"

Agent::Agent(boost::shared_ptr<RNG> rng, const Point2D &dims):
  rng(rng),
  dims(dims)
{}

Agent::~Agent() {
}

std::string Agent::generateLongDescription(unsigned int indentation) {
  return indent(indentation) + generateDescription();
}

void Agent::learn(const Observation &, const Observation &, unsigned int ) {
  // do nothing in the common case
}
