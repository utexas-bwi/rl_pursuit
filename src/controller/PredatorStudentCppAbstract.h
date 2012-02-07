#ifndef PREDATOR_ONRMEGUW
#define PREDATOR_ONRMEGUW

struct MoveDistribution {
  MoveDistribution();

  float probNoop; // probability of staying still
  float probUp; // probability of moving up, change by [0,-1]
  float probDown; // probability of moving down, change by [0,1]
  float probLeft; // probability of moving left, change by [-1,0]
  float probRight; // probability of moving right, change by [1,0]

  void setMoveProb(int dx, int dy, float prob);
};

class PredatorStudentCppAbstract {
protected:
  int dims[2];

public:
	PredatorStudentCppAbstract(int dims[2]);

	virtual MoveDistribution step(int pos[2], int preyPosition[2], int predatorPositions[4][2]) = 0;

  void differenceToPosition(int pos1[2], int pos2[2], int diff[2]); // calculates the difference (dx,dy) between points, taking the torus into account, putting the result in diff
  void projectPosition(int pos[2]); // moves the position into the grid by wrapping around the torus
  void movePosition(const int pos[2], const int move[2], int result[2]); // applies a move to a position, wrapping around the torus, putting the result in result
};

#endif /* end of include guard: PREDATOR_ONRMEGUW */
