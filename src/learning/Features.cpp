#include "Features.h"
#include "Common.h"

void getFeatureValues(FeatureType_t i, std::vector<unsigned int> &values) {
  using namespace FeatureType;
  switch (i) {
    case PredInd:
      values = {0,1,2,3};
      break;
    case Prey_dx:
    case Prey_dy:
    case Pred0_dx:
    case Pred0_dy:
    case Pred1_dx:
    case Pred1_dy:
    case Pred2_dx:
    case Pred2_dy:
    case Pred3_dx:
    case Pred3_dy:
      values.clear();
      break;
    case Occupied_0:
    case Occupied_1:
    case Occupied_2:
    case Occupied_3:
    case NextToPrey:
      values = {0,1};
      break;
    case MyHistoricalAction_0:
    case MyHistoricalAction_1:
      values = {0,1,2,3,4,5};
      break;
    case Pred_act:
      values = {0,1,2,3,4};
      break;
    default:
      assert(false);
  }
}

namespace FeatureType {
  //bool initFeatures() {
  void getFeatures(std::vector<Feature> &features) {
    features.resize(FeatureType::NUM);
    for (unsigned int i = 0; i < FeatureType::NUM; i++) {
      features[i].feat = (FeatureType_t)i;
      getFeatureValues((FeatureType_t)i,features[i].values);
      features[i].numeric = (features[i].values.size() == 0);
    }
    //return true;
  }
}
