#include "WorldMultiModelMDP.h"

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
