/*
File: ModelUpdater.cpp
Author: Samuel Barrett
Description: abstract class for updating the models
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "ModelUpdater.h"
#include <boost/lexical_cast.hpp>

ModelUpdater::ModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions):
  rng(rng),
  mdp(mdp),
  models(models),
  modelProbs(modelPrior),
  modelDescriptions(modelDescriptions)
{
}

void ModelUpdater::set(const ModelUpdater &other) {
  modelDescriptions = other.modelDescriptions;
  modelProbs = other.modelProbs;
  removedModelInds = other.removedModelInds;
  for (unsigned int i = 0; i < other.models.size(); i++) {
  }
  models = other.models;
  std::cout << "SET SUCKS" << std::endl;
}

void ModelUpdater::copyModel(unsigned int ind, Model &model) {
  AgentDummy *dummy;
  for (unsigned int i = 0; i < models[ind].size(); i++) {
    dummy = dynamic_cast<AgentDummy*>(models[ind][i].get());
    if (dummy != NULL)
      model.push_back(models[ind][i]);
    else
      model.push_back(boost::shared_ptr<Agent>(models[ind][i]->clone()));
  }
}

void ModelUpdater::selectModel(const State_t &state) {
  unsigned int ind = selectModelInd(state);
  Model model;
  copyModel(ind,model);
  mdp->setAgents(model);
  //mdp->setAgents(models[ind]);
}

void ModelUpdater::normalizeModelProbs(std::vector<double> &modelProbs) {
  double total = 0;
  for (unsigned int i = 0; i < modelProbs.size(); i++)
    total += modelProbs[i];
  for (unsigned int i = 0; i < modelProbs.size(); i++)
    modelProbs[i] /= total;
}

void ModelUpdater::removeModel(unsigned int ind) {
  models.erase(models.begin()+ind,models.begin()+ind+1);
  modelProbs.erase(modelProbs.begin()+ind,modelProbs.begin()+ind+1);
  modelDescriptions.erase(modelDescriptions.begin()+ind,modelDescriptions.begin()+ind+1);

  unsigned int originalInd = ind;
  std::vector<unsigned int>::iterator it;
  for (it = removedModelInds.begin(); it != removedModelInds.end(); it++) {
    if (*it <= originalInd)
      originalInd++;
    else
      break;
  }
  removedModelInds.insert(it,originalInd);
}

std::string ModelUpdater::generateDescription(unsigned int indentation) {
  std::string msg = indent(indentation) + "ModelUpdater " + generateSpecificDescription() + ":\n";
  for (unsigned int i = 0; i < modelDescriptions.size(); i++)
    msg += indent(indentation+1) + modelDescriptions[i] + ": " + boost::lexical_cast<std::string>(modelProbs[i]) + "\n";
  return msg;
}

std::vector<double> ModelUpdater::getBeliefs() {
  return modelProbs;
}

void ModelUpdater::updateControllerInformation(const Observation &obs) {
  std::cout << "START UPDATE CONTROLLER INFO" << std::endl;
  float reward;
  State_t state;
  bool terminal;
  for (unsigned int i = 0; i < models.size(); i++) {
    mdp->setState(getStateFromObs(mdp->getDims(),obs));
    mdp->setAgents(models[i]);
    mdp->takeAction(Action::NOOP,reward,state,terminal);
  }
  std::cout << "STOP UPDATE CONTROLLER INFO" << std::endl;
}
