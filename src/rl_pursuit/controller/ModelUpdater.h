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
#include <rl_pursuit/common/RNG.h>
#include <rl_pursuit/controller/WorldMDP.h>
#include <rl_pursuit/controller/Agent.h>
#include <rl_pursuit/controller/State.h>

struct ModelInfo {
  ModelInfo(const boost::shared_ptr<WorldMDP> &mdp, const std::string &description, double prob);
  boost::shared_ptr<WorldMDP> mdp;
  std::string description;
  double prob;
};

class ModelUpdater {
public:

  ModelUpdater(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models);

  void set(const ModelUpdater &other);
  virtual void updateRealWorldAction(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) = 0;
  virtual void updateSimulationAction(const Action::Type &action, const State_t &state) = 0;
  virtual void learnControllers(const Observation &prevObs, const Observation &currentObs);
  boost::shared_ptr<WorldMDP> selectModel(const State_t &state);
  std::string generateDescription(unsigned int indentation = 0);
  std::vector<double> getBeliefs();
  void updateControllerInformation(const Observation &obs);
  //void copyModel(unsigned int ind, Model &model,boost::shared_ptr<Agent> adhocAgent = boost::shared_ptr<Agent>()) const;
  void normalizeModelProbs();
  void normalizeProbs(std::vector<double> &modelProbs);
  void setPreyPos(const Point2D &preyPos);
  State_t getState(const Observation &obs);

  void enableOutput(const boost::shared_ptr<std::ostream> &outputStream);
  void disableOutput();
  void output();
  
  void enablePrecisionOutput(const boost::shared_ptr<std::ostream> &outputStream);
  void disablePrecisionOutput();

protected:
  virtual unsigned int selectModelInd(const State_t &state) = 0;
  void removeModel(unsigned int ind);
  virtual std::string generateSpecificDescription() = 0;

protected:
  boost::shared_ptr<RNG> rng;
  std::vector<ModelInfo> models;
  std::vector<bool> modelStillUsed;
  boost::shared_ptr<std::ostream> outputStream;
  boost::shared_ptr<std::ostream> precisionOutputStream;
};

#endif /* end of include guard: MODELUPDATER_82ED5P8 */
