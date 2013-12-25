#ifndef STATEMAPPING_H_WATHCNZF
#define STATEMAPPING_H_WATHCNZF

#include <boost/shared_ptr.hpp>

template<class State>
class StateMapping {
public:
  typedef boost::shared_ptr<StateMapping<State> > Ptr;

  StateMapping() {}
  virtual ~StateMapping() {}

  virtual void map(State &state) = 0;
};

#endif /* end of include guard: STATEMAPPING_H_WATHCNZF */

