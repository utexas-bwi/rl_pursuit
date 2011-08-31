#include <controller/AStar.h>
#include <vector>
#include <iostream>

int main(int argc, const char *argv[])
{
  AStar astar(Point2D(5,5));
  std::vector<Point2D> obstacles;
  Point2D current(0,0);
  Point2D goal(3,3);
  obstacles.push_back(Point2D(4,0));
  obstacles.push_back(Point2D(0,4));
  obstacles.push_back(Point2D(1,0));
  obstacles.push_back(Point2D(3,2));
  obstacles.push_back(Point2D(4,3));
  obstacles.push_back(Point2D(2,3));
  obstacles.push_back(Point2D(3,1));
  obstacles.push_back(Point2D(2,1));
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      if (Point2D(i,j) == goal) {
        std::cout << "G";
        continue;
      } else if ( Point2D(i,j) == current) {
        std::cout << "S";
        continue;
      }
      bool obstacle = false;
      for (unsigned int k = 0; k < obstacles.size(); k++) {
        if (obstacles[k] == Point2D(i,j)) {
          obstacle = true;
          break;
        }
      }
      if (obstacle)
        std::cout << "X";
      else
        std::cout << ".";
    }
    std::cout << std::endl;
  }
  std::cout << current << std::endl;
  while(current != goal) {
    astar.plan(current,goal,obstacles);
    if (astar.foundPath()) {
      current = astar.getFirstStep();
      std::cout << current << std::endl;
    } else {
      std::cout << "NO PATH" << std::endl;
      break;
    }
  }
  return 0;
}
