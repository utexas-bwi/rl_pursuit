#include "World.h"
#include <boost/lexical_cast.hpp>

World::World(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world, double actionNoise, bool centerPrey):
  rng(rng),
  world(world),
  dims(world->getDims()),
  actionNoise(actionNoise),
  centerPrey(centerPrey)
{
}

void World::step() {
  step(boost::shared_ptr<std::vector<Action::Type> >(),agents);
}

void World::step(boost::shared_ptr<std::vector<Action::Type> > actions) {
  step(actions,agents);
}

void World::step(std::vector<boost::shared_ptr<Agent> > &agents) {
  step(boost::shared_ptr<std::vector<Action::Type> >(),agents);
}

void World::generateObservation(Observation &obs) {
  world->generateObservation(obs,centerPrey);
}

void World::step(boost::shared_ptr<std::vector<Action::Type> > actions, std::vector<boost::shared_ptr<Agent> > &agents) {
  //std::cout << "START WORLD STEP" << std::endl;
  ActionProbs actionProbs;
  Action::Type action;
  Observation obs;
  std::vector<Point2D> requestedPositions(agents.size());
  
  generateObservation(obs);
  for (unsigned int i = 0; i < agents.size(); i++) {
    actionProbs = getAgentAction(i,agents[i],obs);
    if (!actionProbs.checkTotal()) {
      for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++)
        std::cout << actionProbs[(Action::Type)i] << " ";
      std::cout << std::endl;
    }
    assert(actionProbs.checkTotal());
    action = actionProbs.selectAction(rng);
    if (actions.get() != NULL)
      (*actions)[i] = action;
    requestedPositions[i] = world->getAgentPosition(i,action);
  }

  handleCollisions(requestedPositions);
  //std::cout << "STOP  WORLD STEP" << std::endl;
}

double World::getOutcomeProbApprox(Observation prevObs, const Observation &currentObs, std::vector<boost::shared_ptr<Agent> > &agents) {
  double modelProb = 1.0;
  ActionProbs actionProbs;
  Point2D requestedPosition;
  
  Observation absPrevObs(prevObs);
  Observation absCurrentObs(currentObs);
  // if the prey is centered, uncenter it for the absolute positions
  if (centerPrey) {
    Point2D offset;
    offset = prevObs.absPrey - prevObs.preyPos();
    for (unsigned int i = 0; i < absPrevObs.positions.size(); i++)
      absPrevObs.positions[i] = movePosition(dims,absPrevObs.positions[i],offset);
    
    offset = currentObs.absPrey - currentObs.preyPos();
    for (unsigned int i = 0; i < absCurrentObs.positions.size(); i++)
      absCurrentObs.positions[i] = movePosition(dims,absCurrentObs.positions[i],offset);
  }


  for (unsigned int agentInd = 0; agentInd < agents.size(); agentInd++) {
    //std::cout << "    agent: " << agentInd << std::endl;

    actionProbs = getAgentAction(agentInd,agents[agentInd],prevObs);
    assert(actionProbs.checkTotal());
    double agentProb = 0.0;
    for (unsigned int action = 0; action < Action::NUM_ACTIONS; action++) {
      double prob = actionProbs[(Action::Type)action];
      if (prob == 0)
        continue;
      //std::cout << "      " << Action::MOVES[action] << " " << prob << std::endl;
      // get the requestedPosition
      requestedPosition = movePosition(world->getDims(),absPrevObs.positions[agentInd],(Action::Type)action);
      //std::cout << "      req: " << absPrevObs.positions[agentInd] << "->" << requestedPosition << std::endl;
      
      // did the agent decide to stay still?
      if (requestedPosition == absPrevObs.positions[agentInd]) {
        if (absPrevObs.positions[agentInd] == absCurrentObs.positions[agentInd])
          agentProb += prob;
        continue;
      }
      // get the probability it collided with another agent
      double probOfNoCollision = getProbOfNoCollisionApprox(absPrevObs,absCurrentObs,requestedPosition,agentInd);
      
      if (absCurrentObs.positions[agentInd] != absPrevObs.positions[agentInd]) {
        // if the agent moved, increment by the probability it decided to move * probability it didn't collide
        if (requestedPosition == absCurrentObs.positions[agentInd])
          agentProb += prob * probOfNoCollision;
        continue;
      }
      // the agent stayed still, but tried to move, so what's the probability it collided
      agentProb += prob * (1 - probOfNoCollision);
    } // end for action
    //std::cout << "agentProb: " << agentProb << std::endl;
    modelProb *= agentProb;
    if (modelProb < 1e-90)
      break;
  } // end for agent
  return modelProb;
}

double World::getProbOfNoCollisionApprox(const Observation &prevObs, const Observation &currentObs, const Point2D &requestedPosition, unsigned int agentInd) {
  double probOfNoCollision = 1.0;
  // did it collide?
  int startCollisionInd = -1;
  // collisions with starting positions
  for (unsigned int i = 0; i < prevObs.positions.size(); i++) {
    if (i == agentInd)
      continue;
    //std::cout << requestedPosition << " " << prevObs.positions[i] << std::endl;
    if (prevObs.positions[i] == requestedPosition) {
      probOfNoCollision *= 0.5;
      startCollisionInd = i;
      break;
    }
  }
  
  //std::cout << "---" << std::endl;
  // collisions with end positions
  for (unsigned int i = 0; i < currentObs.positions.size(); i++) {
    if (i == agentInd)
      continue;
    //std::cout << requestedPosition << " " << currentObs.positions[i] << std::endl;
    if (currentObs.positions[i] == requestedPosition) {
      if (startCollisionInd == (int)i)
        probOfNoCollision = 0; // the other agent stayed in place
      else
        probOfNoCollision *= 0.5;
      break;
    }
  }
  return probOfNoCollision;
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
    double noiseWeight = 0;
    for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
      noiseWeight += actionNoise * actionProbs[(Action::Type)i]; 
      actionProbs[(Action::Type)i] *= origWeight;
    }
    noiseWeight /= Action::NUM_ACTIONS;
    for (unsigned int i = 0; i < Action::NUM_ACTIONS; i++) {
      actionProbs[(Action::Type)i] += noiseWeight;
    }
  }

  return actionProbs;
}
