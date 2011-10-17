#ifndef WORLDBELIEFMDP_JF3NQXTY
#define WORLDBELIEFMDP_JF3NQXTY

/*
File: WorldBeliefMDP.h
Author: Samuel Barrett
Description: mdp wrapper of a world, but with beliefs now
Created:  2011-10-04
Modified: 2011-10-04
*/

#include "WorldMDP.h"
#include "ModelUpdaterBayes.h"

class WorldBeliefMDP: public WorldMDP {
public:
  WorldBeliefMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, const StateConverter &stateConverter, boost::shared_ptr<ModelUpdaterBayes> modelUpdater);
  
  virtual void setState(const State_t &state);
  virtual State_t getState(const Observation &obs);
  virtual void takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal);
  virtual void setBeliefs(boost::shared_ptr<ModelUpdater> newModelUpdater);
  virtual std::string generateDescription(unsigned int indentation);

protected:
  boost::shared_ptr<ModelUpdaterBayes> modelUpdater;
  boost::shared_ptr<ModelUpdater> savedModelUpdater;
  StateConverter stateConverter;
  //double time;
};

#endif /* end of include guard: WORLDBELIEFMDP_JF3NQXTY */
