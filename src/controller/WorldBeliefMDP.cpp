/*
File: WorldBeliefMDP.cpp
Author: Samuel Barrett
Description: mdp wrapper of a world, but with beliefs now
Created:  2011-10-04
Modified: 2011-10-04
*/

#include "WorldBeliefMDP.h"

WorldBeliefMDP::WorldBeliefMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, const StateConverter &stateConverter, boost::shared_ptr<WorldMDP> mdp, boost::shared_ptr<ModelUpdaterBayes> modelUpdater):
  WorldMDP(rng,model,controller,adhocAgent),
  mdp(mdp),
  modelUpdater(modelUpdater),
  //mdp(new WorldMDP(rng,model,controller,adhocAgent)),
  //modelUpdater(new ModelUpdaterBayes(rng,mdp,std::vector<std::vector<boost::shared_ptr<Agent> > >(),std::vector<double>(),std::vector<std::string>(),BAYESIAN_UPDATES)),
  prevAction(Action::NUM_ACTIONS),
  stateConverter(stateConverter)
{
  time = 0;
}

void WorldBeliefMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  WorldMDP::takeAction(action,reward,state,terminal);
  // update the beliefs
  Observation obs;
  model->generateObservation(obs);
  if (prevAction < Action::NUM_ACTIONS) {
    double t = getTime();
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
    time += getTime() - t;
  }
  modelUpdater->updateControllerInformation(obs);
  // save information for the modelUpdater
  prevObs = obs;
  prevAction = action;
  // change the state
  stateConverter.convertGeneralStateToBeliefState(state,modelUpdater->getBeliefs());
} 

void WorldBeliefMDP::setBeliefs(boost::shared_ptr<ModelUpdater> newModelUpdater) {
  std::cout << "BELIEF TIME: " << time << std::endl;
  time = 0;
  modelUpdater->set(*newModelUpdater);
}

std::string WorldBeliefMDP::generateDescription(unsigned int indentation) {
  std::string msg = indent(indentation) + "WorldBeliefMDP:\n";
  msg += stateConverter.generateDescription(indentation + 1) + "\n";
  msg += controller->generateDescription(indentation + 1);
  return msg;
}
