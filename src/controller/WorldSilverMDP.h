#ifndef WORLDSILVERMDP_JTPRNJWO
#define WORLDSILVERMDP_JTPRNJWO

/*
File: WorldSilverMDP.h
Author: Samuel Barrett
Description: an mdp wrapper of a world for Silver's work
Created:  2011-09-06
Modified: 2011-09-07
*/

#include <vector>
#include <boost/unordered_map.hpp>
#include "WorldMultiModelMDP.h"

struct StateHelper {
  std::vector<unsigned int> possibleControllers;

  virtual unsigned int sampleControllers(boost::shared_ptr<RNG> rng);
  virtual void addControllers(unsigned int ind);
};

class WorldSilverMDP: public WorldMultiModelMDP {
public:
  WorldSilverMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, const std::vector<std::string> &modelDescriptions, ModelUpdateType modelUpdateType);
  virtual std::string generateDescription(unsigned int indentation = 0);
protected:
  virtual void selectModel();
  virtual void updateModels(const Observation &, Action::Type, const Observation &) {} // DO NOTHING FOR THIS
  virtual void takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal);

protected:
  //std::map<State_t,boost::shared_ptr<StateHelper> > stateHelpers;
  boost::unordered_map<State_t,boost::shared_ptr<StateHelper> > stateHelpers;
  bool firstStateSet;
  unsigned int currentControllerInd;
  virtual boost::shared_ptr<StateHelper> createStateHelper();
};

#endif /* end of include guard: WORLDSILVERMDP_JTPRNJWO */
