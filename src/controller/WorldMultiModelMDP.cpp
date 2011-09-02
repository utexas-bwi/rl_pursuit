#include "WorldMultiModelMDP.h"
  
const double WorldMultiModelMDP::MIN_MODEL_PROB = 0.001;

WorldMultiModelMDP::WorldMultiModelMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<double> &agentModelProbs, ModelUpdateType modelUpdateType):
  WorldMDP(rng,model,controller,adhocAgent),
  agentModelList(agentModelList),
  agentModelProbs(agentModelProbs),
  modelUpdateType(modelUpdateType)
{
  assert(agentModelList.size() == agentModelProbs.size());
  normalizeModelProbs();
}
  
void WorldMultiModelMDP::setState(const State_t &state) {
  WorldMDP::setState(state);
  selectModel();
}

void printModelProbs(const std::vector<double> &modelProbs) {
  for (unsigned int i = 0; i < modelProbs.size(); i++) {
    std::cout << modelProbs[i] << " ";
  }
}

void WorldMultiModelMDP::updateModels(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  // done if we're down to 1 model
  if (agentModelProbs.size() == 1)
    return;
  std::cout << "Pre: ";
  printModelProbs(agentModelProbs);
  std::cout << std::endl;

  std::vector<double> newModelProbs(agentModelProbs);

  // calculate the new model probabilities
  getNewModelProbs(prevObs,lastAction,currentObs,newModelProbs);
  // check if all zero probs
  if (allProbsTooLow(newModelProbs)) {
    std::cout << "All model probs too low" << std::endl;
    return;
  }
  // set our models
  agentModelProbs.swap(newModelProbs);
  // normalize the probabilities
  normalizeModelProbs();
  // delete models with very low probabilities
  removeLowProbabilityModels();
  // if down to one model, set it in the world controller
  if (agentModelProbs.size() == 1)
    controller->setAgentControllers(agentModelList[0]);
  std::cout << "Post: ";
  printModelProbs(agentModelProbs);
  std::cout << std::endl;
}


void WorldMultiModelMDP::selectModel() {
  if (agentModelProbs.size() == 1)
    return;
  float val = rng->randomFloat();
  double total = 0;
  unsigned int selectedInd;
  for (selectedInd = 0; selectedInd < agentModelProbs.size(); selectedInd++) {
    total += agentModelProbs[selectedInd];
    if (val < total)
      break;
  }
  //std::cout << "using model: " << selectedInd << std::endl;
  controller->setAgentControllers(agentModelList[selectedInd]);
}

void WorldMultiModelMDP::normalizeModelProbs() {
  double total = 0; 
  for (unsigned int i = 0; i < agentModelProbs.size(); i++)
    total += agentModelProbs[i];
  for (unsigned int i = 0; i < agentModelProbs.size(); i++)
    agentModelProbs[i] /= total;
}

void WorldMultiModelMDP::getNewModelProbs(const Observation &prevObs, Action::Type lastAction, const Observation &currentObs, std::vector<double> &newModelProbs) {
  double modelProb;
  double loss;
  double eta = 0.5; // eta must be <= 0.5
  for (unsigned int i = 0; i < agentModelProbs.size(); i++) {
    modelProb = calculateModelProb(i,prevObs,lastAction,currentObs);
    switch(modelUpdateType) {
      case BAYESIAN_UPDATES:
        newModelProbs[i] *= modelProb;
        break;
      case POLYNOMIAL_WEIGHTS:
        // TODO Look at this again
        loss = 1.0 - modelProb;
        newModelProbs[i] *= (1 - eta * loss);
        break;
    }
  }
}

double WorldMultiModelMDP::calculateModelProb(unsigned int modelInd, const Observation &prevObs, Action::Type lastAction, const Observation &currentObs) {
  controller->setAgentControllers(agentModelList[modelInd]);
  adhocAgent->setAction(lastAction);
  double prob = controller->getOutcomeProb(prevObs,currentObs);
  //std::cout << "CALCULATE MODEL PROB FOR " << modelInd << " = " << prob << std::endl;
  return prob;
}

bool WorldMultiModelMDP::allProbsTooLow(const std::vector<double> &newModelProbs) {
  double total = 0;
  for (unsigned int i = 0; i < newModelProbs.size(); i++)
    total += newModelProbs[i];
  return (total < newModelProbs.size() * MIN_MODEL_PROB);
}

void WorldMultiModelMDP::removeLowProbabilityModels() {
  unsigned int i = 0;
  bool removedModels = false;
  while (i < agentModelProbs.size()) {
    if (agentModelProbs[i] < MIN_MODEL_PROB) {
      // remove it from the probs and models
      agentModelProbs.erase(agentModelProbs.begin()+i,agentModelProbs.begin()+i+1);
      agentModelList.erase(agentModelList.begin()+i,agentModelList.begin()+i+1);
      removedModels = true;
    } else {
      ++i;
    }
  }
  // if we removed models, we should renormalize
  if (removedModels)
    normalizeModelProbs();
}
