/*
File: ModelUpdater.cpp
Author: Samuel Barrett
Description: abstract class for updating the models
Created:  2011-09-21
Modified: 2011-09-21
*/

#include "ModelUpdater.h"
#include <boost/lexical_cast.hpp>
  
ModelInfo::ModelInfo(const boost::shared_ptr<WorldMDP> &mdp, const std::string &description, double prob):
  mdp(mdp),
  description(description),
  prob(prob)
{
}

ModelUpdater::ModelUpdater(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo> &models):
  rng(rng),
  models(models),
  modelStillUsed(models.size(),true)
{
  normalizeModelProbs();
}

void ModelUpdater::set(const ModelUpdater &other) {
  models.clear();
  for (unsigned int i = 0; i < other.models.size(); i++)
    models.push_back(ModelInfo(other.models[i].mdp->clone(),other.models[i].description,other.models[i].prob));
}

//void ModelUpdater::copyModel(unsigned int ind, Model &model, boost::shared_ptr<Agent> adhocAgent) const {
  //model.clear();
  //if (adhocAgent == NULL)
    //adhocAgent = mdp->getAdhocAgent();
  //for (unsigned int i = 0; i < models[ind].size(); i++) {
    //if (models[ind][i] == mdp->getAdhocAgent())
      //model.push_back(adhocAgent);
    //else
      //model.push_back(boost::shared_ptr<Agent>(models[ind][i]->clone()));
  //}
//}

void ModelUpdater::learnControllers(const Observation &prevObs, const Observation &currentObs) {
  for (unsigned int i = 0; i < models.size(); i++)
    models[i].mdp->learnControllers(prevObs,currentObs);
  //Observation absPrevObs(prevObs);
  //Observation absCurrentObs(currentObs);
  //absPrevObs.uncenterPrey(mdp->getDims());
  //absCurrentObs.uncenterPrey(mdp->getDims());

  //for (unsigned int i = 0; i < models.size(); i++) {
    //for (unsigned int j = 0; j < models[i].size(); j++) {
      //models[i][j]->learn(absPrevObs,absCurrentObs,j);
    //}
  //}
}

boost::shared_ptr<WorldMDP> ModelUpdater::selectModel(const State_t &state) {
  unsigned int ind = selectModelInd(state);
  //boost::shared_ptr<WorldMDP> mdp(new WorldMDP(*(models[ind].mdp)));
  boost::shared_ptr<WorldMDP> mdp = models[ind].mdp->clone();
  mdp->setState(state);
  return mdp;
  //std::cout << "SELECT MODEL: " << modelDescriptions[ind] << std::endl;
  //mdp->setAgents(models[ind]);
}

void ModelUpdater::normalizeModelProbs() {
  double total = 0;
  for (unsigned int i = 0; i < models.size(); i++)
    total += models[i].prob;
  //std::cout << "TOTAL: " << total << std::endl;
  for (unsigned int i = 0; i < models.size(); i++) {
    models[i].prob /= total;
    //std::cout << "  " << i << ": " << modelProbs[i] << std::endl;
  }
}

void ModelUpdater::normalizeProbs(std::vector<double> &modelProbs) {
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
  msg += indent(indentation+1) + "SHORT:\n";
  for (unsigned int i = 0; i < models.size(); i++)
    msg += indent(indentation+2) + models[i].description + ": " + boost::lexical_cast<std::string>(models[i].prob) + "\n";
  msg += indent(indentation+1) + "LONG:\n";
  for (unsigned int i = 0; i < models.size(); i++)
    msg += models[i].mdp->generateDescription(indentation+2);
  return msg;
}

std::vector<double> ModelUpdater::getBeliefs() {
  std::vector<double> probs(modelStillUsed.size(),0);
  unsigned int ind = 0;
  for (unsigned int i = 0; i < modelStillUsed.size(); i++) {
    if (modelStillUsed[i]) {
      probs[i] = models[ind].prob;
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
    models[i].mdp->setState(obs);
    models[i].mdp->step(Action::NOOP);
    //mdp->setAgents(models[i]);
    //mdp->takeAction(Action::NOOP,reward,state,terminal);
  }
  // reset the mdp
  //mdp->setState(obs);
  //std::cout << "STOP UPDATE CONTROLLER INFO" << std::endl;
}

void ModelUpdater::setPreyPos(const Point2D &preyPos) {
  for (unsigned int i = 0; i < models.size(); i++)
    models[i].mdp->setPreyPos(preyPos);
}

State_t ModelUpdater::getState(const Observation &obs) {
  return models[0].mdp->getState(obs);
}

void ModelUpdater::enableOutput(const boost::shared_ptr<std::ostream> &outputStream) {
  this->outputStream = outputStream;
}

void ModelUpdater::disableOutput() {
  outputStream.reset();
}

void ModelUpdater::output() {
  if (outputStream.get() == NULL)
    return;
  std::ostream &out = *outputStream;
  out << "{";
  for (unsigned int i = 0; i < models.size(); i++) {
    if (i != 0)
      out << ",";
    out << '"' << models[i].description << "\":" << models[i].prob;
  }
  out << "}";
  out << std::endl;
}

void ModelUpdater::enablePrecisionOutput(const boost::shared_ptr<std::ostream> &outputStream) {
  this->precisionOutputStream = outputStream;
}

void ModelUpdater::disablePrecisionOutput() {
  precisionOutputStream.reset();
}
