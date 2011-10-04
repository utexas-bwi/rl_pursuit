#include "PredatorMCTS.h"

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<WorldMDP> model, boost::shared_ptr<ModelUpdater> modelUpdater):
  Agent(rng,dims),
  planner(planner),
  model(model),
  modelUpdater(modelUpdater),
  prevAction(Action::NUM_MOVES)
{}

ActionProbs PredatorMCTS::step(const Observation &obs) {
  // update the probabilities of the models
  if (prevAction < Action::NUM_MOVES) {
    std::cout << "START UPDATE PROBS" << std::endl;
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
    std::cout << "STOP  UPDATE PROBS" << std::endl;
  }
  // set the beliefs of the model (applicable for the belief mdp)
  model->setBeliefs(modelUpdater);
  // do the searching
  State_t state = getStateFromObs(dims,obs);
  std::cout << "-------------------" << std::endl;
  planner->search(state);
  std::cout << "-------------------" << std::endl;
  // save information for the modelUpdater
  prevAction = planner->selectWorldAction(state);
  prevObs = obs;
  
  // update the internal models to the current state
  std::cout << "HERE" << std::endl;
  modelUpdater->updateControllerInformation(obs);
  std::cout << "HERE2" << std::endl;
  
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
