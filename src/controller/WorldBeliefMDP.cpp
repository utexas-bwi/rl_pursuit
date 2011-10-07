/*
File: WorldBeliefMDP.cpp
Author: Samuel Barrett
Description: mdp wrapper of a world, but with beliefs now
Created:  2011-10-04
Modified: 2011-10-04
*/

#include "WorldBeliefMDP.h"

WorldBeliefMDP::WorldBeliefMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, const StateConverter &stateConverter, boost::shared_ptr<ModelUpdaterBayes> modelUpdater):
  WorldMDP(rng,model,controller,adhocAgent),
  modelUpdater(modelUpdater),
  prevAction(Action::NUM_ACTIONS),
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
  // also reset what we know about previous actions
  prevAction = Action::NUM_ACTIONS;
}

State_t WorldBeliefMDP::getState(const Observation &obs) {
  State_t state = WorldMDP::getState(obs);
  stateConverter.convertGeneralStateToBeliefState(state,modelUpdater->getBeliefs());
  //std::cout << "worldbeliefmdp getState: " << state << std::endl;
  return state;
}

void WorldBeliefMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  Observation origObs;
  Observation obs;

  model->generateObservation(origObs);
  //std::cout << "world beliefMDP obs at start: " << origObs << std::endl;
  WorldMDP::takeAction(action,reward,state,terminal);
  model->generateObservation(obs);
  //std::cout << "world beliefMDP obs after action: " << obs << std::endl;
  
  //std::cout << modelUpdater->generateDescription(1) << std::endl;
  // update the beliefs
  if (prevAction < Action::NUM_ACTIONS) {
    //double t = getTime();
    //std::cout << "start belief model update" << std::endl;
    modelUpdater->updateRealWorldAction(prevObs,prevAction,origObs);
    //std::cout << "stop  belief model update" << std::endl;
    //time += getTime() - t;
  }
  //std::cout << modelUpdater->generateDescription(1) << std::endl;
  model->generateObservation(obs);
  //std::cout << "world beliefMDP obs after its model updater: " << obs << std::endl;
  //std::cout << "start update UCI" << std::endl;
  modelUpdater->updateControllerInformation(origObs);
  WorldMDP::setState(obs);
  //std::cout << "stop  update UCI" << std::endl;
  // save information for the modelUpdater
  prevObs = origObs;
  prevAction = action;
  // change the state
  //stateConverter.convertGeneralStateToBeliefState(state,modelUpdater->getBeliefs());
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
