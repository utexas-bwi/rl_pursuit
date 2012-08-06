#ifndef FEATURES_YY6H8O9W
#define FEATURES_YY6H8O9W

#include <common/Enum.h>
#include <cmath>

const float FEATURE_UNSET = 999999;
inline bool isFeatureUnset(float val) {
  return fabs(FEATURE_UNSET - val) < 0.0001f;
}

// order of these do matter to some degree
// order prey immediately before predators, dx before dy
ENUM(FeatureType,
  //Trial,
  //Step,
  PredInd,
  Prey_dx,
  Prey_dy,
  Pred0_dx,
  Pred0_dy,
  Pred1_dx,
  Pred1_dy,
  Pred2_dx,
  Pred2_dy,
  Pred3_dx,
  Pred3_dy,
  Occupied_0,
  Occupied_1,
  Occupied_2,
  Occupied_3,
  NextToPrey,
  MyHistoricalAction_0,
  MyHistoricalAction_1,
  Pred_act
)

namespace FeatureType {
  const int numCategories[NUM] = {
    //0, //Trial,
    //0, //Step,
    4, //PredInd,
    0, //Prey_dx,
    0, //Prey_dy,
    0, //Pred0_dx,
    0, //Pred0_dy,
    0, //Pred1_dx,
    0, //Pred1_dy,
    0, //Pred2_dx,
    0, //Pred2_dy,
    0, //Pred3_dx,
    0, //Pred3_dy,
    2, //Occupied_0,
    2, //Occupied_1,
    2, //Occupied_2,
    2, //Occupied_3,
    2, //NextToPrey,
    5, //MyHistoricalAction_0,
    5, //MyHistoricalAction_1,
    5 //Pred_act
  };
}

#endif /* end of include guard: FEATURES_YY6H8O9W */
