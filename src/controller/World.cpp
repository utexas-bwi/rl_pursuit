#include "World.h"
#include <boost/lexical_cast.hpp>

World::World(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world, double actionNoise):
  rng(rng),
  world(world),
  dims(world->getDims()),
  actionNoise(actionNoise)
{
}

void World::step() {
  step(agents);
}

void World::step(std::vector<boost::shared_ptr<Agent> > &agents) {
  //std::cout << "START WORLD STEP" << std::endl;
  ActionProbs actionProbs;
  Action::Type action;
  Observation obs;
  std::vector<Point2D> requestedPositions(agents.size());
  
  world->generateObservation(obs);
  for (unsigned int i = 0; i < agents.size(); i++) {
    actionProbs = getAgentAction(i,agents[i],obs);
    assert(actionProbs.checkTotal());
    action = actionProbs.selectAction(rng);
    requestedPositions[i] = world->getAgentPosition(i,action);
  }

  handleCollisions(requestedPositions);
  //std::cout << "STOP  WORLD STEP" << std::endl;
}

double World::getOutcomeProbApprox(Observation prevObs, const Observation &currentObs, std::vector<boost::shared_ptr<Agent> > &agents) {
  double modelProb = 1.0;
  ActionProbs actionProbs;
  Point2D requestedPosition;
  for (unsigned int agentInd = 0; agentInd < agents.size(); agentInd++) {
    //std::cout << "    agent: " << agentInd << std::endl;
    double probOfNoCollision = 1.0;

    actionProbs = getAgentAction(agentInd,agents[agentInd],prevObs);
    assert(actionProbs.checkTotal());
    double agentProb = 0.0;
    for (unsigned int action = 0; action < Action::NUM_ACTIONS; action++) {
      double prob = actionProbs[(Action::Type)action];
      //std::cout << "      " << action << " " << prob << std::endl;
      if (prob == 0)
        continue;
      // get the requestedPosition
      requestedPosition = movePosition(world->getDims(),prevObs.positions[agentInd],(Action::Type)action);

      if (currentObs.positions[agentInd] != prevObs.positions[agentInd]) {
        //std::cout << "    agent moved: " << prevObs.positions[agentInd] << " -> " << currentObs.positions[agentInd] << std::endl;
        // the agent moved, so life is simple
        // if the requestedPosition matches what happened, then it works with full prob
        // otherwise, zero prob
        if (requestedPosition == currentObs.positions[agentInd])
          agentProb += prob;
        continue;
      }
      // the agent didn't move
      
      // did the agent decide to stay still?
      if (requestedPosition == currentObs.positions[agentInd]) {
        agentProb += prob;
        continue;
      }
  
      // did it collide?
      // collisions with starting positions
      for (unsigned int i = 0; i < prevObs.positions.size(); i++) {
        if (prevObs.positions[i] == requestedPosition)
          probOfNoCollision *= 0.5;
      }

      // collisions with end positions
      for (unsigned int i = 0; i < currentObs.positions.size(); i++) {
        if (currentObs.positions[i] == requestedPosition)
          probOfNoCollision *= 0.5;
      }

      agentProb += prob * (1 - probOfNoCollision);
    } // end for action
    modelProb *= agentProb;
  } // end for agent
  return modelProb;
}

double World::getOutcomeProb(Observation prevObs,const Observation &currentObs) {
  assert(false); 
  // DON'T USE THIS FUNCTION, it's currently buggy, not sure how so, but I don't trust it, drops some probs to 0 that I think it shouldn't
  double modelProb = 0.0;
  std::vector<ActionProbs> actionProbs(agents.size());
  for (unsigned int i = 0; i < agents.size(); i++) {
    actionProbs[i] = getAgentAction(i,agents[i],prevObs);
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
  //std::cout << "START SETTING AGENT CONTROLLERS" << std::endl;
  if (newAgents.size() != agents.size())
    std::cout << "bad sizes: " << agents.size() << " " << newAgents.size() << std::endl;
  assert(newAgents.size() == agents.size());
  for (unsigned int i = 0; i < newAgents.size(); i++) {
    agents[i] = newAgents[i];
    //std::cout << typeid(*agents[i]).name() << std::endl;
  }
  //std::cout << "STOP  SETTING AGENT CONTROLLERS" << std::endl;
}

std::string World::generateDescription(unsigned int indentation) {
  std::string s;
  s += indent(indentation) + "World:\n";
  s += indent(indentation+1) + "Action Noise: " + boost::lexical_cast<std::string>(actionNoise) + "\n";
  s += world->generateDescription(indentation+1) + "\n";
  s += indent(indentation+1) + "Agents:\n";
  for (unsigned int i = 0; i < agents.size(); i++)
    s += agents[i]->generateLongDescription(indentation+2) + "\n";
  return s;
}

void World::printAgents() {
  for (unsigned int i = 0; i < agents.size(); i++) {
    std::cout << typeid(*agents[i]).name() << std::endl;
  }
}

ActionProbs World::getAgentAction(unsigned int ind, boost::shared_ptr<Agent> agent, Observation &obs) {
  ActionProbs actionProbs;
  obs.myInd = ind;
  actionProbs = agent->step(obs);

  if (actionNoise > 0) {
    double origWeight = 1 - actionNoise;
    double noiseWeight = actionNoise / Action::NUM_ACTIONS;
    for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
      actionProbs[(Action::Type)i] = origWeight * actionProbs[(Action::Type)i] + noiseWeight;
  }

  return actionProbs;
}
