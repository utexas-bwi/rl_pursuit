#ifndef AGENTMODEL_FSYKG09L
#define AGENTMODEL_FSYKG09L

/*
File: AgentModel.h
Author: Samuel Barrett
Description: an agent in the pursuit domain
Created:  2011-08-22
Modified: 2011-08-22
*/

#include <cassert>
#include <common/Point2D.h>

enum AgentType {
  PREY,
  PREDATOR,
  ADHOC
};

AgentType getAgentType(const std::string &type);

struct AgentModel {
  Point2D pos;
  AgentType type;

  AgentModel(int x, int y, AgentType type);
};

#endif /* end of include guard: AGENTMODEL_FSYKG09L */
