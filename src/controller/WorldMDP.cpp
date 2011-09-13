#include "WorldMDP.h"

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
  //std::cout << "In state: " << state << std::endl;
  //Observation obs;
  //model->generateObservation(obs);
  //std::cout << "PRE: " << obs << std::endl;
  
  std::vector<Point2D> positions(STATE_SIZE);
  getPositionsFromState(state,model->getDims(),positions);
  for (unsigned int i = 0; i < STATE_SIZE; i++)
    model->setAgentPosition(i,positions[i]);
    //model->setAgentPosition(i,state.positions[i]);
  //model->generateObservation(obs);
  //std::cout << "POST: " << obs << std::endl;
  //std::cout << "DONE SET STATE" << std::endl;
  //std::cout << "*******************************" << std::endl;
  rolloutStartState = state;
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
/*
State::State(const Observation &obs) {
  for (unsigned int i = 0; i < STATE_SIZE; i++)
    positions[i] = obs.positions[i];
}

bool State::operator<(const State &other) const{
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    if (positions[i].x < other.positions[i].x)
      return true;
    else if (positions[i].x > other.positions[i].x)
      return false;
    else if (positions[i].y < other.positions[i].y)
      return true;
    else if (positions[i].y > other.positions[i].y)
      return false;
  }
  // equal
  return false;
}

bool State::operator==(const State &other) const{
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    if (positions[i].x != other.positions[i].x)
      return false;
    else if (positions[i].y != other.positions[i].y)
      return false;
  }
  // equal
  return true;
}

std::ostream& operator<<(std::ostream &out, const State &state) {
  out << "<State ";
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    out << state.positions[i];
  }
  out << ">";
  return out;
}

std::size_t hash_value(const State &s) {
  std::size_t seed = 0;
  for (unsigned int i = 0; i < STATE_SIZE; i++) {
    boost::hash_combine(seed,s.positions[i].x);
    boost::hash_combine(seed,s.positions[i].y);
  }
  return seed;
}
*/

State_t getStateFromObs(const Point2D &dims, const Observation &obs) {
  State_t state = 0;
  for (int i = ((int)obs.positions.size())-1; i >= 0; i--) {
    state *= dims.y;
    state += obs.positions[i].x;
    state *= dims.x;
    state += obs.positions[i].y;
  }
  return state;
}

void getPositionsFromState(State_t state, const Point2D &dims, std::vector<Point2D> &positions) {
  for (unsigned int i = 0; i < positions.size(); i++) {
    positions[i].y = state % dims.y;
    state /= dims.y;
    positions[i].x = state % dims.x;
    state /= dims.x;
  }
  assert(state == 0);
}

