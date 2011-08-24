#include "WorldModel.h"
#include "Common.h"

#include <iostream>
#include <cassert>

using std::cerr;
using std::endl;
using Action::NUM_NEIGHBORS;
using Action::MOVES;

WorldModel::WorldModel(const Point2D &dims): 
  dims(dims),
  preyInd(-1)
{
}

bool WorldModel::addAgent(const AgentModel& agent, bool ignorePosition) {
  if (!ignorePosition) {
    if (agent.pos.x > dims.x) {
      cerr << "WorldModel::addAgent: failed due to bad x position: " << agent.pos.x << " > " << dims.x << endl;
      return false;
    } else if (agent.pos.y > dims.y) {
      cerr << "WorldModel::addAgent: failed due to bad y position: " << agent.pos.y << " > " << dims.y << endl;
      return false;
    }

    if (getCollision(agent.pos) >= 0) {
      cerr << "WorldModel::addAgent: failed due to collision" << endl;
      return false;
    }
  }

  if (agent.type == PREY) {
    if (preyInd < 0) {
      preyInd = agents.size();
    } else {
      cerr << "WorldModel::addAgent: failed, only supports a single prey" << endl;
      return false;
    }
  }

  agents.push_back(agent);
  return true;
}

bool WorldModel::isPreyCaptured() const {
  if (preyInd < 0)
    return true;
  for (int i = 0; i < NUM_NEIGHBORS; i++) {
    if (getCollision(getAgentPosition(preyInd,(Action::Type)i)) < 0) {
      return false;
    }
  }
  return true;
}

int WorldModel::getCollision(const Point2D& pos, int skipInd, int maxInd) const {
  if ((maxInd < 0) || ((unsigned int)maxInd > agents.size()))
    maxInd = agents.size();
  for (int i = 0; i < maxInd; i++) {
    if ((i != skipInd) && (pos == agents[i].pos))
      return i;
  }
  return -1;
}

Point2D WorldModel::getAgentPosition(unsigned int ind, Action::Type action) const {
  return movePosition(dims,agents[ind].pos,action);
}

void WorldModel::generateObservation(Observation &obs) const {
  obs.preyInd = preyInd;
  obs.myInd = 0;
  obs.positions.clear();
  for (unsigned int i = 0; i < agents.size(); i++)
    obs.positions.push_back(agents[i].pos);
}

void WorldModel::setPositionsFromObservation(const Observation &obs) {
  for (unsigned int i = 0; i < agents.size(); i++)
    setAgentPosition(i,obs.positions[i]);
}
