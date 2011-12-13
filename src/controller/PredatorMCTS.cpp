#include "PredatorMCTS.h"

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<WorldMDP> model, boost::shared_ptr<ModelUpdater> modelUpdater):
  Agent(rng,dims),
  planner(planner),
  model(model),
  modelUpdater(modelUpdater),
  prevAction(Action::NUM_MOVES)
{}

ActionProbs PredatorMCTS::step(const Observation &obs) {
  //std::cout << "REAL WORLD STATE: " << obs << std::endl;
  // update the probabilities of the models
  if (prevAction < Action::NUM_MOVES) {
    //std::cout << "start predmcts model update" << std::endl;
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
    modelUpdater->learnControllers(prevObs,obs);
    //std::cout << "stop  predmcts model update" << std::endl;
  }
  planner->pruneOldVisits(1); // remove everything we didn't see last set of rollouts
  // output the model updater's probabilities
  modelUpdater->output();
  // set the beliefs of the model (applicable for the belief mdp)
  model->setBeliefs(modelUpdater);
  model->setPreyPos(obs.absPrey);
  // do the searching
  State_t state = model->getState(obs);
  //std::cout << "----------START SEARCH---------" << std::endl;
  planner->search(state);
  //std::cout << "----------STOP  SEARCH---------" << std::endl;
  // save information for the modelUpdater
  prevAction = planner->selectWorldAction(state);
  prevObs = obs;
  
  // update the internal models to the current state
  //std::cout << "update controller information: " << obs << std::endl;
  modelUpdater->updateControllerInformation(obs);
  
  return ActionProbs(prevAction);
}

void PredatorMCTS::restart() {
  // only necessary for determining types
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
