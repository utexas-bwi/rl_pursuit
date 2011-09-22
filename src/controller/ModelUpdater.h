#ifndef MODELUPDATER_82ED5P8
#define MODELUPDATER_82ED5P8

/*
File: ModelUpdater.h
Author: Samuel Barrett
Description: abstract class for updating the models
Created:  2011-09-21
Modified: 2011-09-21
*/

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>
#include <common/RNG.h>
#include <controller/WorldMDP.h>
#include <controller/Agent.h>
#include <controller/State.h>

class ModelUpdater {
public:
  typedef std::vector<boost::shared_ptr<Agent> > Model;

  ModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions);

  virtual void updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) = 0;
  virtual void updateSimulationAction(const Action::Type &action, const State_t &state) = 0;
  void selectModel(const State_t &state);
  std::string generateDescription(unsigned int indentation = 0);

protected:
  virtual unsigned int selectModelInd(const State_t &state) = 0;
  void normalizeModelProbs(std::vector<double> &modelProbs);
  void removeModel(unsigned int ind);
  virtual std::string generateSpecificDescription() = 0;

protected:
  boost::shared_ptr<RNG> rng;
  boost::shared_ptr<WorldMDP> mdp;
  boost::shared_ptr<AgentDummy> adhocAgent;
  std::vector<Model> models;
  std::vector<double> modelProbs;
  std::vector<std::string> modelDescriptions;
};

#endif /* end of include guard: MODELUPDATER_82ED5P8 */
