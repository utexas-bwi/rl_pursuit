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
  modelDescriptions(modelDescriptions),
  modelStillUsed(models.size(),true)
{
}

void ModelUpdater::set(const ModelUpdater &other) {
  modelDescriptions = other.modelDescriptions;
  modelProbs = other.modelProbs;
  modelStillUsed = other.modelStillUsed;
  models.clear();
  Model model;
  for (unsigned int i = 0; i < other.models.size(); i++) {
    other.copyModel(i,model,mdp->getAdhocAgent());
    models.push_back(model);
  }
}

void ModelUpdater::copyModel(unsigned int ind, Model &model, boost::shared_ptr<Agent> adhocAgent) const {
  model.clear();
  if (adhocAgent == NULL)
    adhocAgent = mdp->getAdhocAgent();
  for (unsigned int i = 0; i < models[ind].size(); i++) {
    if (models[ind][i] == mdp->getAdhocAgent())
      model.push_back(adhocAgent);
    else
      model.push_back(boost::shared_ptr<Agent>(models[ind][i]->clone()));
  }
}

void ModelUpdater::selectModel(const State_t &state) {
  unsigned int ind = selectModelInd(state);
  Model model;
  copyModel(ind,model);
  mdp->setAgents(model);
  //std::cout << "SELECT MODEL: " << modelDescriptions[ind] << std::endl;
  //mdp->setAgents(models[ind]);
}

void ModelUpdater::normalizeModelProbs(std::vector<double> &modelProbs) {
  double total = 0;
  for (unsigned int i = 0; i < modelProbs.size(); i++)
    total += modelProbs[i];
  //std::cout << "TOTAL: " << total << std::endl;
  for (unsigned int i = 0; i < modelProbs.size(); i++) {
    modelProbs[i] /= total;
    //std::cout << "  " << i << ": " << modelProbs[i] << std::endl;
  }
}

void ModelUpdater::removeModel(unsigned int ind) {
  models.erase(models.begin()+ind,models.begin()+ind+1);
  modelProbs.erase(modelProbs.begin()+ind,modelProbs.begin()+ind+1);
  modelDescriptions.erase(modelDescriptions.begin()+ind,modelDescriptions.begin()+ind+1);

  for (unsigned int i = 0; i < modelStillUsed.size(); i++) {
    if ((modelStillUsed[i]) && (i == ind)) {
      modelStillUsed[i] = false;
      break;
    }
    if (!modelStillUsed[i])
      ind++;
  }
}

std::string ModelUpdater::generateDescription(unsigned int indentation) {
  std::string msg = indent(indentation) + "ModelUpdater " + generateSpecificDescription() + ":\n";
  for (unsigned int i = 0; i < modelDescriptions.size(); i++)
    msg += indent(indentation+1) + modelDescriptions[i] + ": " + boost::lexical_cast<std::string>(modelProbs[i]) + "\n";
  return msg;
}

std::vector<double> ModelUpdater::getBeliefs() {
  std::vector<double> probs(modelStillUsed.size(),0);
  unsigned int ind = 0;
  for (unsigned int i = 0; i < modelStillUsed.size(); i++) {
    if (modelStillUsed[i]) {
      probs[i] = modelProbs[ind];
      ind++;
    }
  }
  return probs;
}

void ModelUpdater::updateControllerInformation(const Observation &obs) {
  //std::cout << "START UPDATE CONTROLLER INFO" << std::endl;
  //std::cout << "UCI: " << mdp.get() << " " << mdp->model.get() << std::endl;
  //float reward;
  //State_t state;
  //bool terminal;
  for (unsigned int i = 0; i < models.size(); i++) {
    mdp->setState(obs);
    mdp->step(Action::NOOP,models[i]);
    //mdp->setAgents(models[i]);
    //mdp->takeAction(Action::NOOP,reward,state,terminal);
  }
  // reset the mdp
  //mdp->setState(obs);
  //std::cout << "STOP UPDATE CONTROLLER INFO" << std::endl;
}
