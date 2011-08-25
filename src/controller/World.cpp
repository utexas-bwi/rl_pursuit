#include "World.h"

World::World(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world):
  rng(rng),
  world(world),
  dims(world->getDims())
{
}

void World::step() {
  Action::Type action;
  Observation obs;
  std::vector<Point2D> requestedPositions;

  world->generateObservation(obs);
  for (unsigned int i = 0; i < agents.size(); i++) {
    obs.myInd = i;
    action = agents[i]->step(obs);
    requestedPositions.push_back(world->getAgentPosition(i,action));
  }

  handleCollisions(requestedPositions);
  //for (unsigned int i = 0; i < agents.size(); i++) {
    //world->setAgentPosition(i,requestedPositions[i]);
  //}
}

void World::handleCollisions(std::vector<Point2D> &requestedPositions) {
  // ORDERED COLLISION DECISION
  unsigned int ind;
  std::vector<unsigned int> agentOrder(agents.size());
  rng->randomOrdering(agentOrder);

  for (unsigned int i = 0; i < agents.size(); i++) {
    ind = agentOrder[i];
    // do nothing on a collision
    if (world->getCollision(requestedPositions[ind],ind) < 0)
      world->setAgentPosition(ind,requestedPositions[ind]);
  }
}

void World::randomizePositions() {
  bool collision;
  Point2D pos;
  for (unsigned int i = 0; i < agents.size(); i++) {
    do {
      pos.x = rng->randomInt(dims.x);
      pos.y = rng->randomInt(dims.y);
      collision = (world->getCollision(pos,-1,i) > 0);
    } while (collision);
    world->setAgentPosition(i,pos);
  } // for loop
}

void World::restartAgents() {
  for (unsigned int i = 0; i < agents.size(); i++) {
    agents[i]->restart();
  }
}

bool World::addAgent(const AgentModel &agentModel, boost::shared_ptr<Agent> agent, bool ignorePosition) {
  bool res = world->addAgent(agentModel,ignorePosition);
  if (!res) {
    return false;
  }
  agents.push_back(agent);
  return true;
}

boost::shared_ptr<const WorldModel> World::getModel() {
  return world;
}

std::string World::generateDescription(unsigned int indentation) {
  std::string s;
  s += indent(indentation) + "World:\n";
  s += world->generateDescription(indentation+1) + "\n";
  s += indent(indentation+1) + "Agents:\n";
  for (unsigned int i = 0; i < agents.size(); i++)
    s += agents[i]->generateLongDescription(indentation+2) + "\n";
  return s;
}
