/*
File: AStar.cpp
Author: Samuel Barrett
Description: an implementation of the A* path planning algorithm for use on a toroidal grid world with obstacles
Created:  2011-08-30
Modified: 2011-08-30
*/

#include "AStar.h"
#include <algorithm>
#include <model/Common.h>
    
AStar::Node::Node(unsigned int gcost, unsigned int hcost, const Point2D &pos, Node *parent):
  gcost(gcost),
  hcost(hcost),
  pos(pos),
  parent(parent)
{
}

bool AStar::Node::operator<(const Node &other) {
  return gcost + hcost > other.gcost + other.hcost; // reversed to put the lowest cost node first
}

AStar::AStar(const Point2D &dims):
  dims(dims)
{
}

void AStar::plan(const Point2D &start, const Point2D &goal, const std::vector<Point2D> &obstacles) {
  // start the open nodes
  openHeap.clear();
  openHeap.push_back(Node(0,0,start,NULL));
  // set obstacles as closed nodes
  for (unsigned int i = 0; i < obstacles.size(); i++)
    closedNodes.insert(obstacles[i]);

  Point2D pos;
  while (openNodes.size() > 0) {
    for (unsigned int i = 0; i < Action::NUM_MOVES; i++) {
      Node node = openHeap.front();
      std::pop_heap(openHeap.begin(),openHeap.end());
      openHeap.pop_back();
      openNodes.erase(node.pos);
      closedNodes.insert(node.pos);
      pos = movePosition(dims,node.pos,(Action::Type)i);
      if (closedNodes.count(pos) == 0) {
        // not closed
      }
    }
  }
}
