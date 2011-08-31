#ifndef ASTAR_LJWWVJXM
#define ASTAR_LJWWVJXM

/*
File: AStar.h
Author: Samuel Barrett
Description: an implementation of the A* path planning algorithm for use on a toroidal grid world with obstacles
Created:  2011-08-30
Modified: 2011-08-31
*/

#include <vector>
#include <set>
#include <common/Point2D.h>

class AStar {
public:
  struct Node {
    Node(unsigned int gcost, unsigned int hcost, const Point2D &pos, Node *parent);
    bool operator<(const Node &other);
    unsigned int gcost;
    unsigned int hcost;
    Point2D pos;
    Node *parent;
  };

  struct Nodecmp {
    bool operator() (const Node *node1, const Node *node2) const;
  };

public:
  AStar(const Point2D &dims);
  ~AStar();
  void plan(const Point2D &start, const Point2D &goal, const std::vector<Point2D> &obstacles);
  Point2D getFirstStep();
  bool foundPath();

private:
  const Point2D dims;
  std::vector<Node*> openHeap;
  std::set<Node*,Nodecmp> openNodes;
  std::set<Node*,Nodecmp> closedNodes;
  Point2D goal;
  Node *goalNode;

private:
  Node* makeNode(unsigned int gcost, const Point2D &pos, Node *parent);
  void clear();
};

#endif /* end of include guard: ASTAR_LJWWVJXM */
