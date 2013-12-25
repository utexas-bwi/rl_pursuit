#ifndef IDENTITYSTATEMAPPING_H_AIOSELMN
#define IDENTITYSTATEMAPPING_H_AIOSELMN

#include "StateMapping.h"

template<class State>
class IdentityStateMapping: public StateMapping<State> {
public:
  IdentityStateMapping () {}
  virtual ~IdentityStateMapping() {}

  virtual void map(State &/*state*/) {
    // do nothing
  }
};

#endif /* end of include guard: IDENTITYSTATEMAPPING_H_AIOSELMN */

