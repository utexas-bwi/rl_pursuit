#ifndef WORLDSILVERMDP_JTPRNJWO
#define WORLDSILVERMDP_JTPRNJWO

/*
File: WorldSilverMDP.h
Author: Samuel Barrett
Description: an mdp wrapper of a world for Silver's work
Created:  2011-09-06
Modified: 2011-09-06
*/

#include <vector>
#include <map>
#include "WorldMultiModelMDP.h"

struct StateHelper {
  std::vector<unsigned int> possibleControllers;

  unsigned int sampleControllers(boost::shared_ptr<RNG> rng);
  void addControllers(unsigned int ind);
};

class WorldSilverMDP: public WorldMultiModelMDP {
public:
  WorldSilverMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, ModelUpdateType modelUpdateType);
protected:
  virtual void selectModel();
  virtual void updateModels(const Observation &, Action::Type, const Observation &) {} // DO NOTHING FOR THIS
  virtual void takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal);

protected:
  std::map<State_t,StateHelper> stateHelpers;
  bool firstStateSet;
  unsigned int currentControllerInd;
};

#endif /* end of include guard: WORLDSILVERMDP_JTPRNJWO */
