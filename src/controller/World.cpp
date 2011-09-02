#include "World.h"

World::World(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world):
  rng(rng),
  world(world),
  dims(world->getDims())
{
}

void World::step() {
  ActionProbs actionProbs;
  Action::Type action;
  Observation obs;
  std::vector<Point2D> requestedPositions(agents.size());

  world->generateObservation(obs);
  for (unsigned int i = 0; i < agents.size(); i++) {
    obs.myInd = i;
    actionProbs = agents[i]->step(obs);
    assert(actionProbs.checkTotal());
    action = actionProbs.selectAction(rng);
    requestedPositions[i] = world->getAgentPosition(i,action);
  }

  handleCollisions(requestedPositions);
}

double World::getOutcomeProb(Observation prevObs,const Observation &currentObs) {
  double modelProb = 0.0;
  std::vector<ActionProbs> actionProbs(agents.size());
  for (unsigned int i = 0; i < agents.size(); i++) {
    prevObs.myInd = i;
    actionProbs[i] = agents[i]->step(prevObs);
    assert(actionProbs[i].checkTotal());
  }
  // get the number of orderings
  unsigned int numOrderings = 1;
  for (unsigned int i = 2; i < agents.size(); i++)
    numOrderings *= i;

  std::vector<unsigned int> actionInds(agents.size(),0);
  std::vector<Point2D> requestedPositions(agents.size());
  double prob;
  bool resultingPositionsMatch;
  // initial the agent order
  std::vector<unsigned int> agentOrder(agents.size());
  for (unsigned int i = 0; i < agents.size(); i++)
    agentOrder[i] = i;

  do { // loop through the possible actions
    // set the current positions
    for (unsigned int i = 0; i < agents.size(); i++)
      world->setAgentPosition(i,prevObs.positions[i]);
    // get the requested positions if the action probability is non-zero
    if (!getRequestedPositionsForActionIndices(actionInds,actionProbs,requestedPositions))
      continue;
    //std::cout << "actionInds: ";
    //for (unsigned int i = 0; i < agents.size(); i++)
      //std::cout << actionInds[i] << " ";
    //std::cout << std::endl;
    do { // loop through the possible agent orderings
      // set the current positions
      for (unsigned int i = 0; i < agents.size(); i++)
        world->setAgentPosition(i,prevObs.positions[i]);
      //std::cout << "origPositions: ";
      //for (unsigned int i = 0; i < agents.size(); i++)
        //std::cout << world->getAgentPosition(i) << " ";
      //std::cout << std::endl;
      // handle collisions
      handleCollisionsOrdered(requestedPositions,agentOrder);
      // check the resulting positions
      resultingPositionsMatch = true;
      //std::cout << "resultingPositions: ";
      //for (unsigned int i = 0; i < agents.size(); i++)
        //std::cout << world->getAgentPosition(i) << " ";
      //std::cout << std::endl;
      //std::cout << "currentPositions: " << currentObs << std::endl;
      for (unsigned int i = 0; i < agents.size(); i++) {
        if (world->getAgentPosition(i) != currentObs.positions[i]) {
          resultingPositionsMatch = false;
          break;
        }
      }
      if (!resultingPositionsMatch)
        continue;
      //std::cout << "RESULTING POSITIONS MATCH" << std::endl;
      // increment the outcome probability
      prob = 1.0;
      for (unsigned int i = 0; i < agents.size(); i++)
        prob *= actionProbs[i][(Action::Type)actionInds[i]];
      modelProb += prob;
    } while (std::next_permutation(agentOrder.begin(),agentOrder.end()));
  } while (incrementActionIndices(actionInds));
  return modelProb / numOrderings;
}

bool World::incrementActionIndices(std::vector<unsigned int> &actionInds) {
  for (unsigned int i = 0; i < actionInds.size(); i++) {
    actionInds[i]++;
    if (actionInds[i] >= Action::NUM_ACTIONS) {
      actionInds[i] = 0;
    } else {
      return true;
    }
  }
  return false;
}
  
bool World::getRequestedPositionsForActionIndices(const std::vector<unsigned int> &actionInds, const std::vector<ActionProbs> &actionProbs, std::vector<Point2D> &requestedPositions) {
  for (unsigned int i = 0; i < agents.size(); i++) {
    if (actionProbs[i][(Action::Type)actionInds[i]] <= 0)
      return false;
    requestedPositions[i] = world->getAgentPosition(i,(Action::Type)actionInds[i]);
  }
  return true;
}

void World::handleCollisions(const std::vector<Point2D> &requestedPositions) {
  // ORDERED COLLISION DECISION
  std::vector<unsigned int> agentOrder(agents.size());
  rng->randomOrdering(agentOrder);
  handleCollisionsOrdered(requestedPositions,agentOrder);
}

void World::handleCollisionsOrdered(const std::vector<Point2D> &requestedPositions, const std::vector<unsigned int> &agentOrder) {
  unsigned int ind;
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
      collision = (world->getCollision(pos,-1,i) >= 0);
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

void World::setAgentControllers(const std::vector<boost::shared_ptr<Agent> > newAgents) {
  assert(newAgents.size() == agents.size());
  for (unsigned int i = 0; i < newAgents.size(); i++)
    agents[i] = newAgents[i];
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
