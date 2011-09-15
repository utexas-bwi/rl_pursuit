#ifndef WORLDSILVERWEIGHTEDMDP_GR3YYQH4
#define WORLDSILVERWEIGHTEDMDP_GR3YYQH4

/*
File: WorldSilverWeightedMDP.h
Author: Samuel Barrett
Description: an mdp wrapper of a world for Silver's work, but using weighted particles
Created:  2011-09-07
Modified: 2011-09-07
*/

#include "WorldSilverMDP.h"

struct StateHelperWeighted: public StateHelper {
  std::vector<unsigned int> controllerCounts;
  unsigned int totalCounts;

  unsigned int sampleControllers(boost::shared_ptr<RNG> rng);
  void addControllers(unsigned int ind);
};

class WorldSilverWeightedMDP: public WorldSilverMDP {
public:
  WorldSilverWeightedMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, const std::vector<std::string> &modelDescriptions, ModelUpdateType modelUpdateType);
  virtual std::string generateDescription(unsigned int indentation = 0);
protected:
  virtual boost::shared_ptr<StateHelper> createStateHelper();
};

#endif /* end of include guard: WORLDSILVERWEIGHTEDMDP_GR3YYQH4 */
