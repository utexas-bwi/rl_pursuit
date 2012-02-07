#include "PredatorStudentCppAbstract.h"

#include <iostream>

MoveDistribution::MoveDistribution():
  probNoop(0.0),
  probUp(0.0),
  probDown(0.0),
  probLeft(0.0),
  probRight(0.0)
{
}

void MoveDistribution::setMoveProb(int dx, int dy, float prob) {
  if ((dx == 0) && (dy == 0))
    probNoop = prob;
  else if ((dx == 0) && (dy == -1))
    probUp = prob;
  else if ((dx == 0) && (dy == 1))
    probDown = prob;
  else if ((dx == -1) && (dy == 0))
    probLeft = prob;
  else if ((dx == 1) && (dy == 0))
    probRight = prob;
  else {
    std::cerr << "Invalid move " << dx << " " << dy << std::endl;
    std::cerr << "Please fix and try again" << std::endl;
    exit(1);
  }
}

PredatorStudentCppAbstract::PredatorStudentCppAbstract(int dims[2]) {
  int i;
  for (i = 0; i < 2; i++)
    this->dims[i] = dims[i];
}

void PredatorStudentCppAbstract::differenceToPosition(int pos1[2], int pos2[2], int diff[2]) {
  float maxDelta;
  for (int i = 0; i < 2; i++) {
    diff[i] = pos2[i] - pos1[i];
    maxDelta = 0.5 * dims[i];
    if (diff[i] > maxDelta)
      diff[i] -= dims[i];
    else if (diff[i] < -maxDelta)
      diff[i] += dims[i];
  }
}

void PredatorStudentCppAbstract::projectPosition(int pos[2]) {
  for (int i = 0; i < 2; i++) {
    if (pos[i] >= dims[i])
      pos[i] -= dims[i];
    else if (pos[i] < 0)
      pos[i] += dims[i];
  }
}

void PredatorStudentCppAbstract::movePosition(const int pos[2], const int move[2], int result[2]) {
  for (int i = 0; i < 2; i++)
    result[i] = pos[i] + move[i];
  projectPosition(result);
}
