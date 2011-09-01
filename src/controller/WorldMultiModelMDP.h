#ifndef WORLDMULTIMODELMDP_V3OI0GS4
#define WORLDMULTIMODELMDP_V3OI0GS4

#include <controller/WorldMDP.h>

enum ModelUpdateType {
  BAYESIAN_UPDATES,
  POLYNOMIAL_WEIGHTS
};

class WorldMultiModelMDP: public WorldMDP {
public:
  WorldMultiModelMDP(boost::shared_ptr<RNG> rng, boost::shared_ptr<WorldModel> model, boost::shared_ptr<World> controller, boost::shared_ptr<AgentDummy> adhocAgent,const std::vector<std::vector<boost::shared_ptr<Agent> > > &agentModelList, const std::vector<float> &agentModelProbs, ModelUpdateType modelUpdateType);
  void setState(const State_t &state);

  void updateModels(const std::vector<Point2D> &lastPositions, Action::Type lastAction, const std::vector<Point2D> &currentPositions);

private:
  void selectModel();
  void normalizeModelProbs();
  void removeLowProbabilityModels();

private:
  std::vector<std::vector<boost::shared_ptr<Agent> > > agentModelList;
  std::vector<float> agentModelProbs;
  ModelUpdateType modelUpdateType;

  const static float MIN_MODEL_PROB;
};

#endif /* end of include guard: WORLDMULTIMODELMDP_V3OI0GS4 */
