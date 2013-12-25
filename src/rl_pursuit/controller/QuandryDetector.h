#ifndef QUANDRYDETECTOR_FOVMENSH
#define QUANDRYDETECTOR_FOVMENSH

#include <boost/circular_buffer.hpp>
#include <rl_pursuit/model/Common.h>
#include <rl_pursuit/common/Params.h>

class QuandryDetector {
public:
#define PARAMS(_) \
  _(unsigned int,historySize,historySize,10) \
  _(unsigned int,notStuckDistMoved,notStuckDistMoved,2)

  Params_STRUCT(PARAMS)
#undef PARAMS

public:
  QuandryDetector(const Point2D &dims, const Params &p);
  
  bool detect(const Observation &obs);

protected:
  Point2D dims;
  Params p;

  boost::circular_buffer<Observation> history;
};

#endif /* end of include guard: QUANDRYDETECTOR_FOVMENSH */
