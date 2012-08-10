#include "World.h"
#include <boost/lexical_cast.hpp>

//#define WORLD_DEBUG

#ifdef WORLD_DEBUG
#define OUTPUT(x) std::cout << x << std::endl
#else
#define OUTPUT(x) ((void) 0)
#endif

/*
bool ObservationComp::operator() (const Observation& lhs, const Observation& rhs) const {
  if (lhs.positions.size() < rhs.positions.size())
    return true;
  else if (lhs.positions.size() > rhs.positions.size())
    return false;
  if (lhs.preyInd < rhs.preyInd)
    return true;
  else if (lhs.preyInd > rhs.preyInd)
    return false;
  for (unsigned int i = 0; i < lhs.positions.size(); i++) {
    if (lhs.positions[i] < rhs.positions[i])
      return true;
    else if (rhs.positions[i] < lhs.positions[i])
      return false;
  }
  return false;
}

std::size_t hash_value(const Observation &o) {
  std::size_t seed = 0;
  boost::hash_combine(seed,o.preyInd);
  for (unsigned int i = 0; i < o.positions.size(); i++) {
    boost::hash_combine(seed,o.positions[i].x);
    boost::hash_combine(seed,o.positions[i].y);
  }
  return seed;
}
*/
World::World(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> world, double actionNoise, bool centerPrey):
  rng(rng),
  world(world),
  dims(world->getDims()),
  actionNoise(actionNoise),
  centerPrey(centerPrey)
  //cachingEnabled(false),
  //uncachedAgentInd(-1),
  //actionCache(new ActionCache())
{
}

void World::generateObservation(Observation &obs) {
  world->generateObservation(obs,centerPrey);
}

void World::step() {
  step(boost::shared_ptr<std::vector<Action::Type> >());//,agents);
}

void World::step(boost::shared_ptr<std::vector<Action::Type> > actions) {
  //step(actions,agents);
//}

//void World::step(std::vector<boost::shared_ptr<Agent> > &agents) {
  //step(boost::shared_ptr<std::vector<Action::Type> >(),agents);
//}

//void World::step(boost::shared_ptr<std::vector<Action::Type> > actions, std::vector<boost::shared_ptr<Agent> > &agents) {
  //std::cout << "START WORLD STEP" << std::endl;
  Action::Type action;
  Observation obs;
  std::vector<Point2D> requestedPositions(agents.size());
  
  generateObservation(obs);

  std::vector<ActionProbs> actionProbList;
/*  
  // check if we already have this in the cache
  if (cachingEnabled) {
    ActionCache::iterator it = actionCache->find(obs);
    if (it != actionCache->end()) {
      //for (unsigned int i = 0; i < agents.size(); i++) {
        //if ((int)i == uncachedAgentInd)
          //continue;
        //ActionProbs p = getAgentAction(i,agents[i],obs);
        //Observation temp(it->first);
        //ActionProbs p2 = getAgentAction(i,agents[i],temp);
        //for (int j = 0; j < 5; j++) {
          //Action::Type a = (Action::Type)j;
          //double val = abs(p[a] - it->second[i][a]);
          //if (val > 0.01) {
            //std::cout << i << " " << j << std::endl;
            //std::cout << "  " << obs << std::endl;
            //std::cout << "  " << it->first << std::endl;
            //std::cout << "  " << val << " " << agents[i]->generateDescription() << std::endl;
            //std::cout << "    " << p[a] << " " << it->second[i][a] << " " << p2[a] << std::endl;
          //}
        //}
      //}
      actionProbList = it->second;
      if (uncachedAgentInd >= 0) {
        actionProbList[uncachedAgentInd] = getAgentAction(uncachedAgentInd,agents[uncachedAgentInd],obs);
      }
      for (unsigned int i = 0; i < agents.size(); i++) {
        if ((int)i == uncachedAgentInd)
          continue;
        obs.myInd = i;
        agents[i]->minimalStep(obs);
      }
    }
  }
*/  
  // get the agents actions if they weren't in the cache
  //if (actionProbList.size() == 0) {
    actionProbList.resize(agents.size());
    for (unsigned int i = 0; i < agents.size(); i++) {
      actionProbList[i] = getAgentAction(i,agents[i],obs);
      OUTPUT("action for " << i << ": " << actionProbList[i]);
      if (!actionProbList[i].checkTotal()) {
        for (unsigned int j = 0; j < Action::NUM_ACTIONS; j++)
          std::cout << actionProbList[i][(Action::Type)j] << " ";
        std::cout << std::endl;
        assert(actionProbList[i].checkTotal());
      }
    }
    //if (cachingEnabled) {
      //obs.myInd = 0;
      //(*actionCache)[obs] = actionProbList;
    //}
  //}

  // now select the actions from the probs
  for (unsigned int i = 0; i < agents.size(); i++) {
    action = actionProbList[i].selectAction(rng);
    if (actions.get() != NULL)
      (*actions)[i] = action;
    requestedPositions[i] = world->getAgentPosition(i,action);
  }

  handleCollisions(requestedPositions);
  
  //std::cout << "STOP  WORLD STEP" << std::endl;
}
/*
void World::setUncachedAgent(boost::shared_ptr<Agent> agent) {
  for (unsigned int i = 0; i < agents.size(); i++) {
    if (agents[i].get() == agent.get()) {
      uncachedAgentInd = (int)i;
      return;
    }
  }
  assert(false);
}
*/
void World::getPossibleOutcomesApprox(std::vector<AgentPtr> &agents, AgentPtr agentDummy, std::vector<std::vector<WorldStepOutcome> > &outcomesByAction) {
  assert(false); // doesn't currently work correctly, assumes fixed ordering of agents
  const double EPS = 0.01;
  Observation obs;
  std::vector<ActionProbs> actionProbList(agents.size());

  // get the agents action distributions
  generateObservation(obs);
  for (unsigned int i = 0; i < agents.size(); i++) {
    actionProbList[i] = getAgentAction(i,agents[i],obs);
  }

  // find the dummy agent if provided
  int agentDummyInd = -1;
  for (unsigned int i = 0; i < agents.size(); i++) {
    if (agentDummy == agents[i]) {
      agentDummyInd = (int)i;
      break;
    }
  }
  assert(agentDummyInd != -1);

  // calculate the probs
  std::vector<WorldStepOutcome> outcomes(1);
  outcomes[0].obs = obs;
  outcomes[0].obs.uncenterPrey(dims);
  outcomes[0].prob = 1.0;
  //for (Action::Type dummyAction = (Action::Type)0; dummyAction <= Action::NUM_ACTIONS; dummyAction = Action::Type(dummyAction+1)) {
  //}
  for (unsigned int i = 0; i < agents.size(); i++) {
    std::cout << i << std::endl;
    std::vector<WorldStepOutcome> newOutcomes;
    for (Action::Type a = Action::Type(0); a < Action::NUM_ACTIONS; a = Action::Type(a + 1)) {
      std::cout << "  " << a << std::endl;
      double actionProb;
      if ((int)i == agentDummyInd) {
        actionProb = 1.0;
      } else {
        actionProb = actionProbList[i][a];
      }
      if (actionProb < EPS)
        continue;
      for (unsigned int j = 0; j < outcomes.size(); j++) {
        std::vector<Point2D> &positions = outcomes[j].obs.positions;
        std::cout << "    considering outcome: " << outcomes[j].prob;
        for (unsigned int k = 0; k < positions.size(); k++)
          std::cout << " " << positions[k];
        std::cout << std::endl;
        Point2D pos = movePosition(dims,positions[i],a);
        // check for collisions
        bool collision = false;
        for (unsigned int k = 0; k < positions.size(); k++) {
          if (k == i)
            continue;
          if (positions[k] == pos) {
            collision = true;
            break;
          }
        } // end for k
        WorldStepOutcome outcome(outcomes[j]);
        outcome.prob *= actionProb;
        if (outcome.prob < EPS)
          continue;
        if ((int)i == agentDummyInd) {
          outcome.agentDummyAction = a;
        }
        if (!collision) {
          outcome.obs.positions[i] = pos;
        }
        // check if this outcome already exists
        bool outcomeExists = false;
        for (unsigned int k = 0; k < newOutcomes.size(); k++) {
          if ((newOutcomes[k].agentDummyAction == outcome.agentDummyAction) && (newOutcomes[k].obs == outcome.obs)) {
            newOutcomes[k].prob += outcome.prob;
            outcomeExists = true;
            break;
          }
        }
        if (!outcomeExists)
          newOutcomes.push_back(outcome);
        std::cout << "    adding outcome: " << newOutcomes.back().prob;
        for (unsigned int k = 0; k < newOutcomes.back().obs.positions.size(); k++)
          std::cout << " " << newOutcomes.back().obs.positions[k];
        std::cout << std::endl;
      }
    }
    std::cout << "setting outcomes" << std::endl;
    outcomes = newOutcomes;
  }
  
  // sort the resulting actions
  std::cout << std::endl;
  std::cout << "result:" << std::endl;
  outcomesByAction.clear();
  outcomesByAction.resize(Action::NUM_ACTIONS);
  for (unsigned int a = 0; a < Action::NUM_ACTIONS; a++) {
    std::cout << a << std::endl;
    for (unsigned int i = 0; i < outcomes.size(); i++) {
      if (outcomes[i].agentDummyAction == (int)a) {
        outcomesByAction[a].push_back(outcomes[i]);
        std::cout << "  " << outcomes[i].prob;
        for (unsigned k = 0; k < outcomes[i].obs.positions.size(); k++)
          std::cout << " " << outcomes[i].obs.positions[k];
        std::cout << std::endl;
      }
    }
  }
}

double World::getOutcomeProbApprox(Observation prevObs, const Observation &currentObs, std::vector<double> &agentProbs) { //, std::vector<boost::shared_ptr<Agent> > &agents) {
  double modelProb = 1.0;
  ActionProbs actionProbs;
  Point2D requestedPosition;

  agentProbs.resize(agents.size());
  
  Observation absPrevObs(prevObs);
  Observation absCurrentObs(currentObs);
  // if the prey is centered, uncenter it for the absolute positions
  if (centerPrey) {
    absPrevObs.uncenterPrey(dims);
    absCurrentObs.uncenterPrey(dims);
  }

  // if prey was captured last time, we need to handle it differently
  bool prevCapture = absCurrentObs.didPreyMoveIllegally(dims,absPrevObs.absPrey);
  //std::cout << absPrevObs << std::endl;
  //std::cout << "prev capture: " << std::boolalpha << prevCapture << std::endl;


  for (unsigned int agentInd = 0; agentInd < agents.size(); agentInd++) {
    // skip prey if it was captured last, (i.e. give it a probability of 1 of doing the observed move)
    if (((int)agentInd == absPrevObs.preyInd) && prevCapture)
      continue;
    //std::cout << "    agent: " << agentInd << std::endl;

    actionProbs = getAgentAction(agentInd,agents[agentInd],prevObs);
    assert(actionProbs.checkTotal());
    //double agentProb = 0.0;
    double &agentProb = agentProbs[agentInd];
    agentProb = 0.0;
    for (unsigned int action = 0; action < Action::NUM_ACTIONS; action++) {
      double prob = actionProbs[(Action::Type)action];
      if (prob == 0)
        continue;
      //std::cout << "      " << Action::MOVES[action] << " " << prob << std::endl;
      // get the requestedPosition
      requestedPosition = movePosition(dims,absPrevObs.positions[agentInd],(Action::Type)action);
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
  //for (unsigned int i = 0; i < agentOrder.size(); i++)
    //agentOrder[i] = i;
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

void World::randomizePreyPosition() {
  Point2D pos;
  bool collision;
  do {
    pos.x = rng->randomInt(dims.x);
    pos.y = rng->randomInt(dims.y);
    collision = (world->getCollision(pos) >= 0); // includes collisions with current location
  } while (collision);
  int preyInd = world->getPreyInd();
  world->setAgentPosition(preyInd,pos);
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

boost::shared_ptr<WorldModel> World::getModel() {
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

boost::shared_ptr<World> World::clone() const {
  boost::shared_ptr<AgentDummy> oldAdhocAgent, newAdhocAgent;
  return clone(oldAdhocAgent,newAdhocAgent);
}

boost::shared_ptr<World> World::clone(const boost::shared_ptr<AgentDummy> &oldAdhocAgent, boost::shared_ptr<AgentDummy> &newAdhocAgent) const {
  boost::shared_ptr<World> controller(new World(rng,world->clone(),actionNoise,centerPrey));
  for (unsigned int i = 0; i < agents.size(); i++) {
    controller->agents.push_back(boost::shared_ptr<Agent>(agents[i]->clone()));
    if (agents[i].get() == oldAdhocAgent.get())
      newAdhocAgent = boost::static_pointer_cast<AgentDummy>(controller->agents.back());
  }
  //controller->cachingEnabled = cachingEnabled;
  //controller->actionCache = actionCache;
  //controller->uncachedAgentInd = uncachedAgentInd;
  return controller;
}
/*
void World::setCaching(bool cachingEnabled) {
  this->cachingEnabled = cachingEnabled;
  // TODO
}
*/  
void World::learnControllers(const Observation &prevObs, const Observation &currentObs) {
  Observation absPrevObs(prevObs);
  Observation absCurrentObs(currentObs);
  absPrevObs.uncenterPrey(dims);
  absCurrentObs.uncenterPrey(dims);

  for (unsigned int i = 0; i < agents.size(); i++) {
    absPrevObs.myInd = i;
    absCurrentObs.myInd = i;
    agents[i]->learn(absPrevObs,absCurrentObs,i);
  }
}
