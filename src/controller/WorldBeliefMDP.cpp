/*
File: WorldBeliefMDP.cpp
Author: Samuel Barrett
Description: mdp wrapper of a world, but with beliefs now
Created:  2011-10-04
Modified: 2011-10-04
*/

#include "WorldBeliefMDP.h"

WorldBeliefMDP::WorldBeliefMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, bool usePreySymmetry, const StateConverter &stateConverter, boost::shared_ptr<ModelUpdaterBayes> modelUpdater):
  WorldMDP(rng,model,controller,adhocAgent,usePreySymmetry),
  modelUpdater(modelUpdater),
  stateConverter(stateConverter)
{
  //time = 0;
}

void WorldBeliefMDP::setState(const State_t &state) {
  //std::cout << "worldbeliefmdp setState: " << state << std::endl;
  State_t generalState = stateConverter.convertBeliefStateToGeneralState(state);
  WorldMDP::setState(generalState);
  // IGNORING THE belief part here, using our saved version
  modelUpdater->set(*savedModelUpdater);
}

State_t WorldBeliefMDP::getState(const Observation &obs) {
  State_t state = WorldMDP::getState(obs);
  //State_t savedState = state;
  stateConverter.convertGeneralStateToBeliefState(state,modelUpdater->getBeliefs());
  //std::vector<double> probs(5,0);
  //stateConverter.convertGeneralStateToBeliefState(state,probs);
  //std::cout << "get state: " << savedState << " ";
  //std::cout << state << std::endl;
  //std::cout << "worldbeliefmdp getState: " << state << std::endl;
  return state;
}

void WorldBeliefMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  Observation prevObs;
  Observation newObs;

  model->generateObservation(prevObs);
  WorldMDP::takeAction(action,reward,state,terminal);
  model->generateObservation(newObs);
  if (!terminal) {
    // update the beliefs
    modelUpdater->updateRealWorldAction(prevObs,action,newObs);
    // update the controllers
    modelUpdater->updateControllerInformation(newObs);
    // set the state back
    WorldMDP::setState(newObs);
  }
  state = getState(newObs);
} 

void WorldBeliefMDP::setBeliefs(boost::shared_ptr<ModelUpdater> newModelUpdater) {
  //std::cout << "BELIEF TIME: " << time << std::endl;
  //time = 0;
  savedModelUpdater = newModelUpdater;
  modelUpdater->set(*savedModelUpdater);
}

std::string WorldBeliefMDP::generateDescription(unsigned int indentation) {
  std::string msg = indent(indentation) + "WorldBeliefMDP:\n";
  msg += stateConverter.generateDescription(indentation + 1) + "\n";
  msg += controller->generateDescription(indentation + 1);
  return msg;
} 
