#ifndef FEATUREEXTRACTOR_XSHC1OD2
#define FEATUREEXTRACTOR_XSHC1OD2

/*
File: FeatureExtractor.h
Author: Samuel Barrett
Description: extracts a set of features of the agents
Created:  2011-10-28
Modified: 2011-12-10
*/

#include <deque>
#include <rl_pursuit/model/Common.h>
#include <rl_pursuit/common/Point2D.h>
#include "Classifier.h"
#include <rl_pursuit/controller/Agent.h>
#include <boost/unordered_map.hpp>
#include <boost/circular_buffer.hpp>

struct FeatureExtractorHistory {
  FeatureExtractorHistory();
  void reset();

  bool initialized;
  std::vector<boost::circular_buffer<Action::Type> > actionHistory;
  Observation obs;
};

class FeatureExtractor {
public:
  FeatureExtractor(const Point2D &dims);
  
  void addFeatureAgent(const std::string &key, const std::string &name);
  InstancePtr extract(const Observation &obs, FeatureExtractorHistory &history);
  void updateHistory(const Observation &obs, FeatureExtractorHistory &history);
  void calcObservedActions(Observation prevObs, Observation obs, std::vector<Action::Type> &actions);
  void printTimes();

protected:
  struct FeatureAgent {
    std::string name;
    boost::shared_ptr<Agent> agent;
  };

  //void setFeature(InstancePtr &instance, const std::string &key, float val);
  inline void setFeature(InstancePtr &instance, unsigned int key, float val) {
    setFeature(instance,(FeatureType_t)key,val);
  }

  inline void setFeature(InstancePtr &instance, FeatureType_t key, float val) {
    (*instance)[key] = val;
  }


protected:
  const Point2D dims;
  std::vector<FeatureAgent> featureAgents;
  std::vector<std::string> featureKeys;
  unsigned int keyInd;

public:
  static const unsigned int HISTORY_SIZE;
  static const bool USE_ALL_AGENTS_HISTORY;
};

#endif /* end of include guard: FEATUREEXTRACTOR_XSHC1OD2 */
