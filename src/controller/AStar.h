#ifndef ASTAR_LJWWVJXM
#define ASTAR_LJWWVJXM

/*
File: AStar.h
Author: Samuel Barrett
Description: an implementation of the A* path planning algorithm for use on a toroidal grid world with obstacles
Created:  2011-08-30
Modified: 2011-08-30
*/

#include <vector>
#include <set>
#include <common/Point2D.h>

class AStar {
  struct Node {
    Node(unsigned int gcost, unsigned int hcost, const Point2D &pos, Node *parent);
    bool operator<(const Node &other);
    unsigned int gcost;
    unsigned int hcost;
    Point2D pos;
    Node *parent;
  };

public:
  AStar(const Point2D &dims);
  void plan(const Point2D &start, const Point2D &goal, const std::vector<Point2D> &obstacles);

private:
  const Point2D dims;
  std::vector<Node> openHeap;
  std::set<Point2D> openNodes;
  std::set<Point2D> closedNodes;
};

#endif /* end of include guard: ASTAR_LJWWVJXM */
