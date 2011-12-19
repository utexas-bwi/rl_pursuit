#ifndef WORLDMODEL_OIVQAWRT
#define WORLDMODEL_OIVQAWRT

/*
File: WorldModel.h
Author: Samuel Barrett
Description: contains the necessary information for a pursuit simulation
Created:  2011-08-22
Modified: 2011-11-16
*/

#include <vector>

#include <json/json.h>
#include <common/Point2D.h>
#include "AgentModel.h"
#include "Common.h"

class WorldModel {
public:
  WorldModel(const Point2D &dims);
  bool addAgent(const AgentModel& agent, bool ignorePosition=false);
  bool isPreyCaptured() const;
  int getCollision(const Point2D &pos, int skipInd = -1, int maxInd = -1) const;
  inline unsigned int getNumAgents() const {return agents.size();}
  inline Point2D getDims() const {return dims;}

  inline void setAgentPosition(unsigned int ind, const Point2D &pos) {agents[ind].pos = pos;}
  Point2D getAgentPosition(unsigned int ind, Action::Type action = Action::NOOP) const;
  void generateObservation(Observation &obs, bool centerPrey) const;
  void setPositionsFromObservation(Observation obs);
  std::string generateDescription(unsigned int indentation = 0);

  virtual boost::shared_ptr<WorldModel> clone() const {
    return boost::shared_ptr<WorldModel>(new WorldModel(*this));
  }

protected:
  const Point2D dims;
  std::vector<AgentModel> agents;
  int preyInd;
  Point2D lastCenterPreyOffset;
};

#endif /* end of include guard: WORLDMODEL_OIVQAWRT */
