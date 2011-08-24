#include "PlanningFactory.h"
#include "WorldFactory.h"

boost::shared_ptr<WorldMDP> createWorldMDP(boost::shared_ptr<RNG> rng, const Point2D &dims, const Json::Value &options) {
  boost::shared_ptr<WorldModel> model = createWorldModel(dims);
  boost::shared_ptr<World> controller = createWorldAgents(rng->randomUInt(),model,options);
  assert(model->getNumAgents() == 4); // no ad hoc agent yet
  boost::shared_ptr<AgentDummy> adhocAgent(new AgentDummy(boost::shared_ptr<RNG>(new RNG(rng->randomUInt())),dims));
  controller->addAgent(AgentModel(0,0,ADHOC),adhocAgent,true);
  return boost::shared_ptr<WorldMDP>(new WorldMDP(rng,model,controller,adhocAgent));
}

///////////////////////////////////////////////////////////////////////////

boost::shared_ptr<UCTEstimator<State_t> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action_t numActions, float lambda, float gamma, float rewardRangePerStep, float initialValue, unsigned int initialStateVisits, unsigned int initialStateActionVisits, float unseenValue) {
  return boost::shared_ptr<UCTEstimator<State_t> >(new UCTEstimator<State_t>(rng,numActions,lambda,gamma,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue));
}

boost::shared_ptr<UCTEstimator<State_t> > createUCTEstimator(boost::shared_ptr<RNG> rng, Action_t numActions, float rewardRangePerStep, const Json::Value &options) {
  float lambda = options.get("lambda",0.8).asDouble();
  float gamma = options.get("gamma",0.95).asDouble();
  float unseenValue = options.get("unseenValue",9999999).asDouble();
  float initialValue = options.get("initialValue",0).asDouble();
  unsigned int initialStateVisits = options.get("initialStateVisits",0).asUInt();
  unsigned int initialStateActionVisits = options.get("initialStateActionVisits",0).asUInt();
  return createUCTEstimator(rng,numActions,lambda,gamma,rewardRangePerStep,initialValue,initialStateVisits,initialStateActionVisits,unseenValue);
}

boost::shared_ptr<UCTEstimator<State_t> > createUCTEstimator(unsigned int randomSeed, Action_t numActions, float rewardRangePerStep, const Json::Value &options) {
  return createUCTEstimator(boost::shared_ptr<RNG>(new RNG(randomSeed)),numActions,rewardRangePerStep,options);
}
