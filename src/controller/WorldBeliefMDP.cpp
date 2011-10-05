/*
File: WorldBeliefMDP.cpp
Author: Samuel Barrett
Description: mdp wrapper of a world, but with beliefs now
Created:  2011-10-04
Modified: 2011-10-04
*/

#include "WorldBeliefMDP.h"

WorldBeliefMDP::WorldBeliefMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, const StateConverter &stateConverter):
  WorldMDP(rng,model,controller,adhocAgent),
  prevAction(Action::NUM_ACTIONS),
  stateConverter(stateConverter)
{
}

void WorldBeliefMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  WorldMDP::takeAction(action,reward,state,terminal);
  // update the beliefs
  Observation obs;
  model->generateObservation(obs);
  if (prevAction < Action::NUM_ACTIONS)
    modelUpdater->updateRealWorldAction(prevObs,prevAction,obs);
  modelUpdater->updateControllerInformation(obs);
  // save information for the modelUpdater
  prevObs = obs;
  prevAction = action;
  // change the state
  stateConverter.convertGeneralStateToBeliefState(state,modelUpdater->getBeliefs());
} 

void WorldBeliefMDP::setBeliefs(boost::shared_ptr<ModelUpdater> newModelUpdater) {
  // FIXME
  modelUpdater->set(*newModelUpdater);
}

