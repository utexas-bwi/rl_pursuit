#ifndef STATEVALUEESTIMATOR_QWHQR9BY
#define STATEVALUEESTIMATOR_QWHQR9BY

#include <string>

template<class State, class Action>
class VIEstimator {
  public:
    VIEstimator () {}
    virtual ~VIEstimator () {}

    virtual float getValue(const State &state) = 0;
    virtual void updateValue(const State &state, float value) = 0;
    virtual Action getBestAction(const State &state) = 0;
    virtual void setBestAction(const State &state, const Action& action) = 0;

    virtual void saveEstimatedValues(const std::string& file) = 0;
    virtual void loadEstimatedValues(const std::string& file) = 0;

    virtual std::string generateDescription(unsigned int indentation = 0) = 0;
};


#endif /* end of include guard: STATEVALUEESTIMATOR_QWHQR9BY */
