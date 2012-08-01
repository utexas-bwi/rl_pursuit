#include "PredatorMCTS.h"

#define PREDATOR_MCTS_TIMING

double PREDATOR_MCTS_TIMING_MODEL_UPDATE = 0.;
double PREDATOR_MCTS_TIMING_SEARCH = 0.;
double PREDATOR_MCTS_TIMING_UPDATE_CONTROLLER = 0.;
double PREDATOR_MCTS_TIMING_PRUNING = 0.;

PredatorMCTS::PredatorMCTS(boost::shared_ptr<RNG> rng, const Point2D &dims, boost::shared_ptr<MCTS<State_t,Action::Type> > planner, boost::shared_ptr<ModelUpdater> modelUpdater):
  Agent(rng,dims),
  planner(planner),
  modelUpdater(modelUpdater),
  prevAction(Action::NUM_MOVES)
{
  PREDATOR_MCTS_TIMING_MODEL_UPDATE = 0.;
  PREDATOR_MCTS_TIMING_SEARCH = 0.;
  PREDATOR_MCTS_TIMING_UPDATE_CONTROLLER = 0.;
  PREDATOR_MCTS_TIMING_PRUNING = 0.;
}

ActionProbs PredatorMCTS::step(const Observation &obs) {
  //std::cout << "REAL WORLD STATE: " << obs << std::endl;
  // update the probabilities of the models
  if (prevAction < Action::NUM_MOVES) {
    //std::cout << "start predmcts model update" << std::endl;
#ifdef PREDATOR_MCTS_TIMING
    tic();
#endif
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
    modelUpdater->learnControllers(prevObs,obs);
#ifdef PREDATOR_MCTS_TIMING
    toc(PREDATOR_MCTS_TIMING_MODEL_UPDATE);
#endif
    //std::cout << "stop  predmcts model update" << std::endl;
  }
#ifdef PREDATOR_MCTS_TIMING
  tic(2);
#endif
  planner->pruneOldVisits(); // remove everything we didn't see last set of rollouts
#ifdef PREDATOR_MCTS_TIMING
  toc(PREDATOR_MCTS_TIMING_PRUNING,2);
#endif
  // output the model updater's probabilities
  modelUpdater->output();
  // set the beliefs of the model (applicable for the belief mdp)
  //model->setBeliefs(modelUpdater); // TODO
  modelUpdater->setPreyPos(obs.absPrey);
  // do the searching
  State_t state = modelUpdater->getState(obs);
  //std::cout << "----------START SEARCH---------" << std::endl;
#ifdef PREDATOR_MCTS_TIMING
  tic(3);
#endif
  planner->search(state);
#ifdef PREDATOR_MCTS_TIMING
  toc(PREDATOR_MCTS_TIMING_SEARCH,3);
#endif
  //std::cout << "----------STOP  SEARCH---------" << std::endl;
  // save information for the modelUpdater
  prevAction = planner->selectWorldAction(state);
  prevObs = obs;
  
  // update the internal models to the current state
  //std::cout << "update controller information: " << obs << std::endl;
#ifdef PREDATOR_MCTS_TIMING
  tic(4);
#endif
  modelUpdater->updateControllerInformation(obs);
#ifdef PREDATOR_MCTS_TIMING
  toc(PREDATOR_MCTS_TIMING_UPDATE_CONTROLLER,4);
#endif

#ifdef PREDATOR_MCTS_TIMING
  std::cout << "Timings (model,prune,search,controller): " << PREDATOR_MCTS_TIMING_MODEL_UPDATE << " " << PREDATOR_MCTS_TIMING_PRUNING << " " << PREDATOR_MCTS_TIMING_SEARCH << " " << PREDATOR_MCTS_TIMING_UPDATE_CONTROLLER << std::endl;
#endif
  
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
