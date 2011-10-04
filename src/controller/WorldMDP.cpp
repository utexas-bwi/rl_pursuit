#include "WorldMDP.h"
#include <controller/ModelUpdater.h>

WorldMDP::WorldMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent):
  rng(rng),
  model(model),
  controller(controller),
  adhocAgent(adhocAgent)
{
}

void WorldMDP::setState(const State_t &state) {
  //std::cout << "*******************************" << std::endl;
  //std::cout << "START SET STATE" << std::endl;
  //std::cout << model.get() << " " << model->getDims() << std::endl;
  //std::cout << "In state: " << state << std::endl;
  //Observation obs;
  //model->generateObservation(obs);
  //std::cout << "PRE: " << obs << std::endl;
  
  std::vector<Point2D> positions(STATE_SIZE);
  //std::cout << "STATE_SIZE: " << STATE_SIZE << std::endl;
  getPositionsFromState(state,model->getDims(),positions);
  for (unsigned int i = 0; i < STATE_SIZE; i++)
    model->setAgentPosition(i,positions[i]);
    //model->setAgentPosition(i,state.positions[i]);
  //model->generateObservation(obs);
  //std::cout << "POST: " << obs << std::endl;
  //std::cout << "DONE SET STATE" << std::endl;
  //std::cout << "*******************************" << std::endl;
}

void WorldMDP::takeAction(const Action::Type &action, float &reward, State_t &state, bool &terminal) {
  adhocAgent->setAction(action);
  controller->step();

  if (model->isPreyCaptured()) {
    reward = 1.0;
    terminal = true;
  } else {
    reward = 0.0;
    terminal = false;
  }

  Observation obs;
  model->generateObservation(obs);
  state = getStateFromObs(model->getDims(),obs);
  //for (unsigned int i = 0; i < STATE_SIZE; i++)
    //state.positions[i] = model->getAgentPosition(i);
}

float WorldMDP::getRewardRangePerStep() {
  return 1.0;
}

std::string WorldMDP::generateDescription(unsigned int indentation) {
  return controller->generateDescription(indentation);
}

void WorldMDP::setAgents(const std::vector<boost::shared_ptr<Agent> > &agents) {
  //for (unsigned int i = 0; i < agents.size(); i++) {
    //std::cout << typeid(*(agents[i].get())).name() << " " << agents[i].get() << std::endl;
  //}
  controller->setAgentControllers(agents);
}

double WorldMDP::getOutcomeProb(const Observation &prevObs, Action::Type adhocAction, const Observation &currentObs) {
  adhocAgent->setAction(adhocAction);
  return controller->getOutcomeProb(prevObs,currentObs);
}

boost::shared_ptr<AgentDummy> WorldMDP::getAdhocAgent() {
  return adhocAgent;
}

void WorldMDP::addAgent(AgentType agentType, boost::shared_ptr<Agent> agent) {
  controller->addAgent(AgentModel(0,0,agentType),agent,true);
}
