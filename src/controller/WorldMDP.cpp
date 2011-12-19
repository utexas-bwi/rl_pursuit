#include "WorldMDP.h"
#include <controller/ModelUpdater.h>
#include <controller/ModelUpdaterBayes.h>
#include <factory/PlanningFactory.h>

const bool WorldMDP::useCaching = true;

WorldMDP::WorldMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent, bool usePreySymmetry):
  rng(rng),
  model(model),
  controller(controller),
  adhocAgent(adhocAgent),
  preyPos(0.5f * model->getDims()),
  usePreySymmetry(usePreySymmetry)
{
}

void WorldMDP::setPreyPos(const Point2D &preyPos) {
  this->preyPos = preyPos;
}

void WorldMDP::setState(const State_t &state) {
  //std::cout << "worldmdp setState: " << state << std::endl;
  Observation obs;
  obs.preyInd = 0;
  obs.positions.resize(STATE_SIZE);
  getPositionsFromState(state,model->getDims(),obs.positions,preyPos,usePreySymmetry);
  obs.absPrey = this->preyPos;
  setState(obs);

  //std::vector<Point2D> positions(STATE_SIZE);
  //getPositionsFromState(state,model->getDims(),positions,preyPos,usePreySymmetry);
  //for (unsigned int i = 0; i < STATE_SIZE; i++)
    //model->setAgentPosition(i,positions[i]);
}

void WorldMDP::setState(const Observation &obs) {
  //std::cout << "worldmdp setstateobs: " << obs << std::endl;
  model->setPositionsFromObservation(obs);
}

State_t WorldMDP::getState(const Observation &obs) {
  State_t state = getStateFromObs(model->getDims(),obs,usePreySymmetry);
  //std::cout << "worldmdp getState: " << state << std::endl;
  return state;
}

void WorldMDP::step(Action::Type adhocAction) {//, std::vector<boost::shared_ptr<Agent> > &agents) {
  adhocAgent->setAction(adhocAction);
  controller->step();//agents);
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
  controller->generateObservation(obs);
  state = getState(obs);
  //std::cout << obs << std::endl;
  //for (unsigned int i = 0; i < STATE_SIZE; i++)
    //state.positions[i] = model->getAgentPosition(i);
}

float WorldMDP::getRewardRangePerStep() {
  return 1.0;
}

std::string WorldMDP::generateDescription(unsigned int indentation) {
  return indent(indentation) + "WorldMDP:\n" + controller->generateDescription(indentation + 1);
}

void WorldMDP::setAgents(const std::vector<boost::shared_ptr<Agent> > &agents) {
  //std::cout << "START SET AGENTS" << std::endl;
  //for (unsigned int i = 0; i < agents.size(); i++) {
    //std::cout << typeid(*(agents[i].get())).name() << " " << agents[i].get() << std::endl;
  //}
  controller->setAgentControllers(agents);
  //currentModel = agents;
  //std::cout << "STOP  SET AGENTS" << std::endl;
}
/*
void WorldMDP::saveAgents() {
  savedModel = currentModel;
}

void WorldMDP::loadAgents() {
  setAgents(savedModel);
}
*/
double WorldMDP::getOutcomeProb(const Observation &prevObs, Action::Type adhocAction, const Observation &currentObs) { //,std::vector<boost::shared_ptr<Agent> > &agents) {
  adhocAgent->setAction(adhocAction);
  double probApprox = controller->getOutcomeProbApprox(prevObs,currentObs);//,agents);
  //double probExact = controller->getOutcomeProb(prevObs,currentObs);
  //std::cout << "probs: " << probApprox << " " << probExact << std::endl;
  return probApprox;
  //return probExact;
}

boost::shared_ptr<AgentDummy> WorldMDP::getAdhocAgent() {
  return adhocAgent;
}

void WorldMDP::addAgent(const AgentModel &agentModel, boost::shared_ptr<Agent> agent) {
  controller->addAgent(agentModel,agent,true);
}

void WorldMDP::addAgents(const std::vector<AgentModel> &agentModels, const std::vector<boost::shared_ptr<Agent> > agents) {
  assert(agentModels.size() == agents.size());
  for (unsigned int i = 0; i < agents.size(); i++)
    addAgent(agentModels[i],agents[i]);
}
  
void WorldMDP::learnControllers(const Observation &prevObs, const Observation &currentObs) {
  Observation absPrevObs(prevObs);
  Observation absCurrentObs(currentObs);
  absPrevObs.uncenterPrey(getDims());
  absCurrentObs.uncenterPrey(getDims());

  for (unsigned int i = 0; i < controller->agents.size(); i++) {
    controller->agents[i]->learn(absPrevObs,absCurrentObs,i);
  }
    //for (unsigned int j = 0; j < models[i].size(); j++) {
      //models[i][j]->learn(absPrevObs,absCurrentObs,j);
    //}
  //}
}
  
boost::shared_ptr<WorldMDP> WorldMDP::clone() const {
  boost::shared_ptr<AgentDummy> newAdhocAgent;
  boost::shared_ptr<World> newController = controller->clone(adhocAgent,newAdhocAgent);
  boost::shared_ptr<WorldMDP> mdp(new WorldMDP(rng,newController->getModel(),newController,newAdhocAgent,usePreySymmetry));
  return mdp;
}

void WorldMDP::setAdhocAgent(boost::shared_ptr<AgentDummy> adhocAgent) {
  this->adhocAgent = adhocAgent;
}
