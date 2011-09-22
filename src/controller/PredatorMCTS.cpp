#include "PredatorMCTS.h"

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<WorldMDP> model, boost::shared_ptr<ModelUpdater> modelUpdater):
  Agent(rng,dims),
  planner(planner),
  model(model),
  modelUpdater(modelUpdater),
  prevAction(Action::NUM_MOVES)
{}

ActionProbs PredatorMCTS::step(const Observation &obs) {
  if (prevAction < Action::NUM_MOVES)
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
  State_t state = getStateFromObs(dims,obs);
  planner->search(state);
  prevAction = planner->selectWorldAction(state);
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
  s += modelUpdater->generateDescription(indentation+1);
  s += planner->generateDescription(indentation+1);
  return s;
}
