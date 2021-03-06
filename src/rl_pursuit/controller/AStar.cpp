/*
File: AStar.cpp
Author: Samuel Barrett
Description: an implementation of the A* path planning algorithm for use on a toroidal grid world with obstacles
Created:  2011-08-30
Modified: 2011-09-09
*/

#include "AStar.h"
#include <algorithm>
#include <rl_pursuit/model/Common.h>
#include <iostream>
    
AStar::Node::Node(unsigned int gcost, unsigned int hcost, const Point2D &pos, boost::shared_ptr<Node>parent):
  gcost(gcost),
  hcost(hcost),
  pos(pos),
  parent(parent)
{
}

bool AStar::Node::operator<(const Node &other) {
  return gcost + hcost > other.gcost + other.hcost; // reversed to put the lowest cost node first
}

std::size_t AStar::Nodehash::operator()(const boost::shared_ptr<Node> &node) const {
  std::size_t seed = 0;
  boost::hash_combine(seed,node->pos.x);
  boost::hash_combine(seed,node->pos.y);
  return seed;
}

// NOTE: for the heap to work correctly, we must specify how to compare Node*'s
bool cmp(const boost::shared_ptr<AStar::Node> &node1, const boost::shared_ptr<AStar::Node> &node2) {
  return node1->gcost + node1->hcost > node2->gcost + node2->hcost; // reversed to put the lowest cost node first
}

bool AStar::Nodeequal::operator() (const boost::shared_ptr<Node> &node1, const boost::shared_ptr<Node> &node2) const {
  return node1->pos == node2->pos;
}
  
AStar::AStar(const Point2D &dims):
  dims(dims)
{
}

AStar::~AStar() {
  clear();
}

void AStar::plan(const Point2D &start, const Point2D &goal, const std::vector<Point2D> &obstacles) {
  // set the goal, and clear the previous plan
  this->goal = goal;
  clear();
  goalNode.reset();

  assert(start != goal);

  boost::shared_ptr<Node> newNode;
  Point2D pos;
  //std::set<boost::shared_ptr<Node> >::iterator it;
  boost::unordered_set<boost::shared_ptr<Node> >::iterator it;
  // start the open nodes
  boost::shared_ptr<Node> node(new Node(0,0,start,boost::shared_ptr<Node>()));
  setHeuristic(node);
  openHeap.push_back(node);
  openNodes.insert(node);
  // set obstacles as closed nodes
  for (unsigned int i = 0; i < obstacles.size(); i++) {
    if (obstacles[i] != goal) // ignore any obstacles at the goal
      closedNodes.insert(boost::shared_ptr<Node>(new Node(0,0,obstacles[i],boost::shared_ptr<Node>())));
  }

  // while there are open nodes
  while (openNodes.size() > 0) {
    node = openHeap.front(); // get the lowest cost node
    if (node->pos == goal) {
      // we're done
      goalNode = node;
      break;
    }
    // move the node from open to closed
    std::pop_heap(openHeap.begin(),openHeap.end(),cmp); // resort the heap
    openHeap.pop_back(); // remove the node from the heap
    openNodes.erase(node); // remove the node from the open set
    closedNodes.insert(node); // add the node to the closed set
    // search its neighbors
    for (unsigned int i = 0; i < Action::NUM_NEIGHBORS; i++) {
      pos = movePosition(dims,node->pos,(Action::Type)i);
      newNode = boost::shared_ptr<Node>(new Node(node->gcost + 1,0,pos,node));
      if (closedNodes.count(newNode) > 0) {
        continue;
      }
      it = openNodes.find(newNode);
      if (it != openNodes.end()) {
        // already open
        if ((*it)->gcost > newNode->gcost) {
          (*it)->gcost = newNode->gcost;
          (*it)->parent = node;
          // have to resort because this item might be out of place now
          std::make_heap(openHeap.begin(),openHeap.end(),cmp);
        }
      } else {
        // new open node
        setHeuristic(newNode);
        openNodes.insert(newNode);
        openHeap.push_back(newNode);
        std::push_heap(openHeap.begin(),openHeap.end(),cmp);
      }
    }
  }
}

Point2D AStar::getFirstStep() {
  boost::shared_ptr<Node>node = goalNode->parent;
  boost::shared_ptr<Node>prev = goalNode;
  while (node->parent != NULL) {
    prev = node;
    node = node->parent;
  }
  return prev->pos;
}

bool AStar::foundPath() {
  return goalNode != NULL;
}

void AStar::setHeuristic(boost::shared_ptr<Node>node) {
  node->hcost = getDistanceToPoint(dims,node->pos,goal);
}

void AStar::clear() {
  openNodes.clear();
  openHeap.clear();
  closedNodes.clear();
}
