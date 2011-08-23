#include "WorldMDP.h"

WorldMDP::WorldMDP(boost::shared_ptr<RNG> rng,const Point2D &dims, const Json::Value &options):
  rng(rng),
  adhocAgent(new AgentDummy(rng,dims))
{
  // add the dummy agent
  worldController->addAgent(AgentModel(0,0,ADHOC),adhocAgent,true);
}

void WorldMDP::setState(const Observation &state) {
  worldModel->setPositionsFromObservation(state);
}

void WorldMDP::takeAction(const Action::Type &action, float &reward, Observation &state, bool &terminal) {
  adhocAgent->setAction(action);
  worldController->step();

  if (worldModel->isPreyCaptured()) {
    reward = 1.0;
    terminal = true;
  } else {
    reward = 0.0;
    terminal = false;
  }

  worldModel->generateObservation(state);
}
