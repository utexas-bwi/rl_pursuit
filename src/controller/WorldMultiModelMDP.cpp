#include "WorldMultiModelMDP.h"
  
const float WorldMultiModelMDP::MIN_MODEL_PROB = 0.001;

WorldMultiModelMDP::WorldMultiModelMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<float> &agentModelProbs, ModelUpdateType modelUpdateType):
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

void WorldMultiModelMDP::updateModels(const std::vector<Point2D> &lastPositions, Action::Type lastAction, const std::vector<Point2D> &currentPositions) {
  // done if we're down to 1 model
  if (agentModelProbs.size() == 1)
    return;

  std::vector<float> newModelProbs(agentModelProbs.size());

  // TODO getNewModelProbs

  // TODO check if all zero out
  
  // set our models
  agentModelProbs.swap(newModelProbs);
  // normalize the probabilities
  normalizeModelProbs();
  // delete models with very low probabilities
  removeLowProbabilityModels();
}


void WorldMultiModelMDP::selectModel() {
  if (agentModelProbs.size() == 1)
    return;
  float val = rng->randomFloat();
  float total = 0;
  unsigned int selectedInd;
  for (selectedInd = 0; selectedInd < agentModelProbs.size(); selectedInd++) {
    total += agentModelProbs[selectedInd];
    if (val < total)
      break;
  }
  std::cout << "using model: " << selectedInd << std::endl;
  controller->setAgentControllers(agentModelList[selectedInd]);
}

void WorldMultiModelMDP::normalizeModelProbs() {
  float total = 0; 
  for (unsigned int i = 0; i < agentModelProbs.size(); i++)
    total += agentModelProbs[i];
  for (unsigned int i = 0; i < agentModelProbs.size(); i++)
    agentModelProbs[i] /= total;
}

void WorldMultiModelMDP::removeLowProbabilityModels() {
  unsigned int i = 0;
  while (i < agentModelProbs.size()) {
    if (agentModelProbs[i] < MIN_MODEL_PROB) {
      // remove it from the probs and models
      agentModelProbs.erase(agentModelProbs.begin()+i,agentModelProbs.begin()+i+1);
      agentModelList.erase(agentModelList.begin()+i,agentModelList.begin()+i+1);
    } else {
      ++i;
    }
  }
}
