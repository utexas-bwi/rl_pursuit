#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>

#include <rl_pursuit/common/Point2D.h>
#include <rl_pursuit/common/RNG.h>
#include <rl_pursuit/common/Util.h>
#include <rl_pursuit/controller/World.h>
#include <rl_pursuit/factory/AgentFactory.h>
#include <rl_pursuit/factory/WorldFactory.h>

std::vector<boost::shared_ptr<Agent> > createModels(boost::shared_ptr<RNG> rng, const char *modelJson, const Point2D &dims) {
  Json::Value options;
  if (! readJson(modelJson,options)) {
    exit(1);
  }
  std::string predatorStr = options.get("predator","greedy").asString();
  const Json::Value predatorOptions = options["predatorOptions"];
  std::vector<boost::shared_ptr<Agent> > agents(4);
  for (unsigned int i = 0; i < agents.size(); i++) {
    agents[i] = createAgent(rng->randomUInt(),dims,predatorStr,0,i,predatorOptions,Json::Value());
  }
  return agents;
}

int main(int argc, const char *argv[]) {
  const char *usage = "Usage: analyzeModel numTrials randomSeed trueModel approxModel [approxModel ...]";
  int minArgs = 4;
  if ((argc <= minArgs)) {
    std::cout << usage << std::endl;
    return 1;
  }

  unsigned int numTrials = boost::lexical_cast<unsigned int>(argv[1]);
  unsigned int randomSeed = boost::lexical_cast<unsigned int>(argv[2]);
  const char *trueModelJson = argv[3];

  boost::shared_ptr<RNG> rng(new RNG(randomSeed));

  // create the models
  Json::Value options;
  if (! readJson(trueModelJson,options)) {
    exit(1);
  }
  boost::shared_ptr<World> world = createWorldAgents(rng->randomUInt(),0,options);
  boost::shared_ptr<WorldModel> model = world->getModel();

  std::vector<std::vector<boost::shared_ptr<Agent> > > approxModelList;
  for (int i = minArgs; i < argc; i++) {
    approxModelList.push_back(createModels(rng,argv[i],model->getDims()));
  }

  unsigned int numModels = approxModelList.size();
  unsigned int numAgents = approxModelList[0].size();
  
  // run the tests
  std::vector<float> totalOverlap(numModels);
  std::vector<float> numCorrect(numModels);

  std::vector<ActionProbs> trueActions(numAgents + 1);
  ActionProbs approxActions;
  Observation obs;

  for (unsigned int trial = 0; trial < numTrials; trial++) {
    //std::cout << "trial: " << trial << std::endl << std::flush;
    std::vector<float> overlap(numModels);
    std::vector<float> correct(numModels);
    world->randomizePositions();
    world->restartAgents();
    for (unsigned int i = 0; i < numModels; i++)
      for (unsigned int j = 0; j < numAgents; j++)
        approxModelList[i][j]->restart();
    unsigned int step = 0;
    while (!model->isPreyCaptured()) {
      step++;
      world->generateObservation(obs);
      world->step(boost::shared_ptr<std::vector<Action::Type> >(),trueActions);
      for (unsigned int i = 0; i < numModels; i++) {
        for (unsigned int j = 0; j < numAgents; j++) {
          obs.myInd = j + 1;
          approxActions = approxModelList[i][j]->step(obs);
          overlap[i] += trueActions[obs.myInd].overlap(approxActions);
          correct[i] += (approxActions.maxAction() == trueActions[obs.myInd].maxAction());
        }
      }
    }
    for (unsigned int i = 0; i < numModels; i++) {
      totalOverlap[i] = (trial / (trial + 1.0)) * totalOverlap[i] + (1 / (trial + 1.0)) * (overlap[i] / (step * numAgents));
      numCorrect[i]   = (trial / (trial + 1.0)) * numCorrect[i]   + (1 / (trial + 1.0)) * (correct[i] / (step * numAgents));
    }
  }
  std::cout << "avgOverlap: " ;
  for (unsigned int i = 0; i < totalOverlap.size(); i++)
    std::cout << totalOverlap[i] << " ";
  std::cout << std::endl;
  std::cout << "avgCorrect: ";
  for (unsigned int i = 0; i < numCorrect.size(); i++)
    std::cout << numCorrect[i] << " ";
  std::cout << std::endl;

  return 0;
}
