/*
File: ModelUpdater.cpp
Author: Samuel Barrett
Description: abstract class for updating the models
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "ModelUpdater.h"

ModelUpdater::ModelUpdater(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldMDP> mdp, const std::vector<Model> &models, const std::vector<double> &modelPrior, const std::vector<std::string> &modelDescriptions):
  rng(rng),
  mdp(mdp),
  models(models),
  modelProbs(modelPrior),
  modelDescriptions(modelDescriptions)
{
}

void ModelUpdater::selectModel(const State_t &state) {
  std::cout << "SELECT MODEL" << std::endl;
  unsigned int ind = selectModelInd(state);
  mdp->setAgents(models[ind]);
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
}
