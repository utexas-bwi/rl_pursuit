#include "PredatorMCTS.h"

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner):
  Agent(rng,dims),
  planner(planner)
{}

Action::Type PredatorMCTS::step(const Observation &obs) {
  planner->search(obs);
  return planner->selectWorldAction(obs);
}

void PredatorMCTS::restart() {
  // TODO, only necessary for determining types
}

std::string PredatorMCTS::generateDescription() {
  return "PredatorMCTS: a predator using MCTS to select actions";
}

std::string PredatorMCTS::generateLongDescription(unsigned int indentation) {
  std::string s = indent(indentation) + generateDescription() + "\n";
  s += planner->generateDescription(indentation+1);
  return s;
}
