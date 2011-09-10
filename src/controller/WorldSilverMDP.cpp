#include "WorldSilverMDP.h"
#include <cmath>
#include <cassert>
  
unsigned int StateHelper::sampleControllers(boost::shared_ptr<RNG> rng) {
  assert(possibleControllers.size() >= 1);
  int ind = rng->randomInt(possibleControllers.size());
  return possibleControllers[ind];
}

void StateHelper::addControllers(unsigned int ind) {
  for (unsigned int i = 0; i < possibleControllers.size(); i++) {
    if (ind == possibleControllers[i])
      return;
  }
  possibleControllers.push_back(ind);
}

WorldSilverMDP::WorldSilverMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, ModelUpdateType modelUpdateType):
  WorldMultiModelMDP(rng,model,controller,adhocAgent,agentModelList,agentModelProbs,modelUpdateType),
  firstStateSet(false)
{
}

std::string WorldSilverMDP::generateDescription(unsigned int indentation) {
  return indent(indentation) + "Silver\n" + WorldMDP::generateDescription(indentation);
}
 
void WorldSilverMDP::selectModel() {
  if (! firstStateSet) {
    firstStateSet = true;
    boost::shared_ptr<StateHelper> helper = createStateHelper();
    for (unsigned int i = 0; i < agentModelProbs.size(); i++) {
      helper->addControllers(i);
    }
    stateHelpers.insert(std::pair<State_t,boost::shared_ptr<StateHelper> >(rolloutStartState,helper));
  }
  //std::cout << "Choosing model for " << rolloutStartState << " from " << std::flush;
  //std::map<State_t,boost::shared_ptr<StateHelper> >::iterator it = stateHelpers.find(rolloutStartState);
  boost::unordered_map<State_t,boost::shared_ptr<StateHelper> >::iterator it = stateHelpers.find(rolloutStartState);
  if (it == stateHelpers.end()) {
    //std::cerr << "WARNING: unseen state: " << rolloutStartState << ", don't know what controllers could reach it, sampling from the initial set" << std::endl;
    currentControllerInd = rng->randomInt(agentModelProbs.size());
  } else {
    currentControllerInd = it->second->sampleControllers(rng);
  }
  controller->setAgentControllers(agentModelList[currentControllerInd]);
  //if (it != stateHelpers.end()) {
    //std::cout << "(";
    //for (unsigned int i = 0; i < it->second.possibleControllers.size(); i++)
      //std::cout << it->second.possibleControllers[i] << " ";
    //std::cout << "): ";
  //}
  //std::cout << currentControllerInd << std::endl;
}

void WorldSilverMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  WorldMultiModelMDP::takeAction(action,reward,state,terminal);
  //std::map<State_t,boost::shared_ptr<StateHelper> >::iterator it = stateHelpers.find(state);
  boost::unordered_map<State_t,boost::shared_ptr<StateHelper> >::iterator it = stateHelpers.find(state);
  if (it == stateHelpers.end()) {
    // previously unseen state
    boost::shared_ptr<StateHelper> helper = createStateHelper();
    helper->addControllers(currentControllerInd);
    stateHelpers.insert(std::pair<State_t,boost::shared_ptr<StateHelper> >(state,helper));

  } else {
    // seen state, add this controller as having reached it
    it->second->addControllers(currentControllerInd);
  }
  //std::cout << "updated " << state << "  with " << currentControllerInd << std::endl;
}

boost::shared_ptr<StateHelper> WorldSilverMDP::createStateHelper() {
  return boost::shared_ptr<StateHelper>(new StateHelper());
}
