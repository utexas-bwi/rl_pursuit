/*
File: WorldSilverWeightedMDP.cpp
Author: Samuel Barrett
Description: an mdp wrapper of a world for Silver's work, but using weighted particles
Created:  2011-09-07
Modified: 2011-09-07
*/

#include "WorldSilverWeightedMDP.h"

unsigned int StateHelperWeighted::sampleControllers(boost::shared_ptr<RNG> rng) {
  float val = rng->randomFloat();
  float total = 0;
  for (unsigned int i = 0; i < controllerCounts.size(); i++) {
    total += controllerCounts[i] / (float)totalCounts;
    if (val < total)
      return possibleControllers[i];
  }
  return possibleControllers.back();
}

void StateHelperWeighted::addControllers(unsigned int ind) {
  for (unsigned int i = 0; i < possibleControllers.size(); i++) {
    if (ind == possibleControllers[i]) {
      controllerCounts[i]++;
      totalCounts++;
      return;
    }
  }
  possibleControllers.push_back(ind);
  controllerCounts.push_back(1);
  totalCounts++;
}

WorldSilverWeightedMDP::WorldSilverWeightedMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, const std::vector<std::string> &modelDescriptions, ModelUpdateType modelUpdateType):
  WorldSilverMDP(rng,model,controller,adhocAgent,agentModelList,agentModelProbs,modelDescriptions,modelUpdateType)
{
}
  
std::string WorldSilverWeightedMDP::generateDescription(unsigned int indentation) {
  return indent(indentation) + "Silver - WEIGHTED\n" + generateModelDescriptions(indentation+1);// + WorldMDP::generateDescription(indentation);
}
  
boost::shared_ptr<StateHelper> WorldSilverWeightedMDP::createStateHelper() {
  return boost::shared_ptr<StateHelper>(new StateHelperWeighted());
}
