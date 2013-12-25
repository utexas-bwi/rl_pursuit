#include "AgentModel.h"
#include <iostream>

AgentType getAgentType(const std::string &type) {
  if (type == "prey")
    return PREY;
  else if (type == "predator")
    return PREDATOR;
  else if (type == "adhoc")
    return ADHOC;
  else {
    std::cerr << "getAgentType: WARNING invalid type specified, using PREDATOR" << std::endl;
    return PREDATOR;
  }
}

AgentModel::AgentModel(int x, int y, AgentType type):
  pos(x,y),
  type(type)
{}
