#include "PredatorMCTS.h"

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<WorldMultiModelMDP> model):
  Agent(rng,dims),
  planner(planner),
  model(model),
  prevAction(Action::NUM_MOVES)
{}

ActionProbs PredatorMCTS::step(const Observation &obs) {
  if (prevAction < Action::NUM_MOVES)
    model->updateModels(prevObs,prevAction,obs);
  planner->search(obs);
  prevAction = planner->selectWorldAction(obs);
  prevObs = obs;
  return ActionProbs(prevAction);
}

void PredatorMCTS::restart() {
  // TODO, only necessary for determining types
  //planner->restart();
  prevAction = Action::NUM_MOVES;
}

std::string PredatorMCTS::generateDescription() {
  return "PredatorMCTS: a predator using MCTS to select actions";
}

std::string PredatorMCTS::generateLongDescription(unsigned int indentation) {
  std::string s = indent(indentation) + generateDescription() + "\n";
  s += planner->generateDescription(indentation+1);
  return s;
}
