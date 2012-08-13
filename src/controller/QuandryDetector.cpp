#include "QuandryDetector.h"

QuandryDetector::QuandryDetector(const Point2D &dims, const Params &p):
  dims(dims),
  p(p),
  history(p.historySize + 1) // +1 because one for the current
{
}

bool QuandryDetector::detect(const Observation &obs) {
  //std::cout << "new obs: " << obs << std::endl;
  history.push_back(obs);
  Observation &current = history.back();
  current.centerPrey(dims);
  //std::cout << "post center obs: " << current << std::endl;
  // not stuck if we just started
  if (history.size() == 1)
    return false;

  // loop through the history and detect if we're stuck
  // currently, the check if if anyone has moved more than notStuckDistMoved relative to the prey
  for (unsigned int historyInd = 0; historyInd < history.size() - 1; historyInd++) { // -1 to skip the current one
    for (unsigned int i = 0; i < current.positions.size(); i++) {
      unsigned int dist = getDistanceToPoint(dims,history[historyInd].positions[i],current.positions[i]);
      if (dist >= p.notStuckDistMoved) {
        std::cout << "not stuck " << historyInd << "/" << history.size() << " " << i << " - " << history[historyInd].positions[i] << " " << current.positions[i] << std::endl;
        return false;
      }
    }
  }

  return true;
}
