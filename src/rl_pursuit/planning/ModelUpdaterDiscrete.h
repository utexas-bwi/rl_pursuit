#ifndef MODELUPDATERDISCRETE_H_IFZHWSUN
#define MODELUPDATERDISCRETE_H_IFZHWSUN

/*
File:     ModelUpdaterDiscrete.h
Author:   Samuel Barrett
Created:  2013-08-08
Modified: 2013-08-08
Description: abstract class for updating a set of discrete models
*/

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include "ModelUpdater.h"
#include <rl_pursuit/common/RNG.h>
#include <rl_pursuit/common/Util.h>

template<class State, class Action>
struct ModelInfo {
  ModelInfo(const boost::shared_ptr<Model<State,Action> > &mdp, const std::string &description, double prob):
    mdp(mdp),
    description(description),
    prob(prob)
  {
  }
  boost::shared_ptr<Model<State,Action> > mdp;
  std::string description;
  double prob;
};

template<class State, class Action>
class ModelUpdaterDiscrete: public ModelUpdater<State,Action> {
public:

  ModelUpdaterDiscrete(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo<State,Action> > &models);

  void set(const ModelUpdaterDiscrete &other);
  virtual void learnControllers(const State &prevState, const State &currentState);
  boost::shared_ptr<Model<State,Action> > selectModel(const State &state);
  std::string generateDescription(unsigned int indentation = 0);
  //std::vector<double> getBeliefs();
  void updateControllerInformation(const State &state);
  void normalizeModelProbs();
  void normalizeProbs(std::vector<double> &modelProbs);

  void output(std::ostream &out);

protected:
  virtual unsigned int selectModelInd(const State &state) = 0;
  void removeModel(unsigned int ind);
  virtual std::string generateSpecificDescription() = 0;

protected:
  boost::shared_ptr<RNG> rng;
  std::vector<ModelInfo<State,Action> > models;
};

//////////////////////////////////////////////////////////////////

#ifdef MODELUPDATER_DEBUG
#define MODELUPDATER_OUTPUT(x) std::cout << x << std::endl
#else
#define MODELUPDATER_OUTPUT(x) ((void) 0)
#endif

template<class State, class Action>
ModelUpdaterDiscrete<State,Action>::ModelUpdaterDiscrete(boost::shared_ptr<RNG> rng, const std::vector<ModelInfo<State,Action> > &models):
  rng(rng),
  models(models)
{
  normalizeModelProbs();
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::set(const ModelUpdaterDiscrete<State,Action> &other) {
  models.clear();
  for (unsigned int i = 0; i < other.models.size(); i++)
    models.push_back(ModelInfo<State,Action>(other.models[i].mdp->clone(),other.models[i].description,other.models[i].prob));
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::learnControllers(const State &prevState, const State &currentState) {
  for (unsigned int i = 0; i < models.size(); i++)
    models[i].mdp->learnControllers(prevState,currentState);
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

template<class State, class Action>
boost::shared_ptr<Model<State,Action> > ModelUpdaterDiscrete<State,Action>::selectModel(const State &state) {
  unsigned int ind = selectModelInd(state);
  //boost::shared_ptr<WorldMDP> mdp(new WorldMDP(*(models[ind].mdp)));
  boost::shared_ptr<Model<State,Action> > mdp = models[ind].mdp->clone();
  mdp->setState(state);
  MODELUPDATER_OUTPUT("Select Model: " << ind << " " << models[ind].description);
  return mdp;
  //std::cout << "SELECT MODEL: " << modelDescriptions[ind] << std::endl;
  //mdp->setAgents(models[ind]);
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::normalizeModelProbs() {
  double total = 0;
  for (unsigned int i = 0; i < models.size(); i++)
    total += models[i].prob;
  for (unsigned int i = 0; i < models.size(); i++) {
    models[i].prob /= total;
  }
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::normalizeProbs(std::vector<double> &modelProbs) {
  double total = 0;
  for (unsigned int i = 0; i < modelProbs.size(); i++)
    total += modelProbs[i];
  for (unsigned int i = 0; i < modelProbs.size(); i++) {
    modelProbs[i] /= total;
  }
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::removeModel(unsigned int ind) {
  models.erase(models.begin()+ind,models.begin()+ind+1);
}
  
  //for (unsigned int i = 0; i < modelStillUsed.size(); i++) {
    //if ((modelStillUsed[i]) && (i == ind)) {
      //modelStillUsed[i] = false;
      //break;
    //}
    //if (!modelStillUsed[i])
      //ind++;
  //}
//}

template<class State, class Action>
std::string ModelUpdaterDiscrete<State,Action>::generateDescription(unsigned int indentation) {
  std::string msg = indent(indentation) + "ModelUpdaterDiscrete " + generateSpecificDescription() + ":\n";
  msg += indent(indentation+1) + "SHORT:\n";
  for (unsigned int i = 0; i < models.size(); i++)
    msg += indent(indentation+2) + models[i].description + ": " + boost::lexical_cast<std::string>(models[i].prob) + "\n";
  msg += indent(indentation+1) + "LONG:\n";
  for (unsigned int i = 0; i < models.size(); i++)
    msg += models[i].mdp->generateDescription(indentation+2);
  return msg;
}

//template<class State, class Action>
//std::vector<double> ModelUpdaterDiscrete<State,Action>::getBeliefs() {
  //std::vector<double> probs(modelStillUsed.size(),0);
  //unsigned int ind = 0;
  //for (unsigned int i = 0; i < modelStillUsed.size(); i++) {
    //if (modelStillUsed[i]) {
      //probs[i] = models[ind].prob;
      //ind++;
    //}
  //}
  //return probs;
//}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::updateControllerInformation(const State &state) {
  //std::cout << "START UPDATE CONTROLLER INFO" << std::endl;
  //std::cout << "UCI: " << mdp.get() << " " << mdp->model.get() << std::endl;
  //float reward;
  //State_t state;
  //bool terminal;
  for (unsigned int i = 0; i < models.size(); i++) {
    models[i].mdp->setState(state);
    models[i].mdp->step(Action::NOOP);
    //mdp->setAgents(models[i]);
    //mdp->takeAction(Action::NOOP,reward,state,terminal);
  }
  // reset the mdp
  //mdp->setState(obs);
  //std::cout << "STOP UPDATE CONTROLLER INFO" << std::endl;
}

template<class State, class Action>
void ModelUpdaterDiscrete<State,Action>::output(std::ostream &out) {
  out << "{";
  for (unsigned int i = 0; i < models.size(); i++) {
    if (i != 0)
      out << ",";
    out << '"' << models[i].description << "\":" << models[i].prob;
  }
  out << "}";
  out << std::endl;
}
#endif /* end of include guard: MODELUPDATERDISCRETE_H_IFZHWSUN */

