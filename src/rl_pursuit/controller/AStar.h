#ifndef ASTAR_LJWWVJXM
#define ASTAR_LJWWVJXM

/*
File: AStar.h
Author: Samuel Barrett
Description: an implementation of the A* path planning algorithm for use on a toroidal grid world with obstacles
Created:  2011-08-30
Modified: 2011-09-09
*/

#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/shared_ptr.hpp>
#include <rl_pursuit/common/Point2D.h>

class AStar {
public:
  struct Node {
    Node(unsigned int gcost, unsigned int hcost, const Point2D &pos, boost::shared_ptr<Node> parent);
    bool operator<(const Node &other);
    unsigned int gcost;
    unsigned int hcost;
    Point2D pos;
    boost::shared_ptr<Node> parent;
  };

  struct Nodeequal {
    bool operator() (const boost::shared_ptr<Node> &node1, const boost::shared_ptr<Node> &node2) const;
  };
  
  struct Nodehash{
    std::size_t operator()(const boost::shared_ptr<Node> &node) const;
  };


public:
  AStar(const Point2D &dims);
  ~AStar();
  void plan(const Point2D &start, const Point2D &goal, const std::vector<Point2D> &obstacles);
  Point2D getFirstStep();
  bool foundPath();

private:
  const Point2D dims;
  std::vector<boost::shared_ptr<Node> > openHeap;
  boost::unordered_set<boost::shared_ptr<Node>, Nodehash, Nodeequal> openNodes;
  boost::unordered_set<boost::shared_ptr<Node>, Nodehash, Nodeequal> closedNodes;
  //std::set<boost::shared_ptr<Node>,Nodecmp> openNodes;
  //std::set<boost::shared_ptr<Node>,Nodecmp> closedNodes;
  Point2D goal;
  boost::shared_ptr<Node> goalNode;

private:
  void setHeuristic(boost::shared_ptr<Node> node);
  void clear();
};

#endif /* end of include guard: ASTAR_LJWWVJXM */
