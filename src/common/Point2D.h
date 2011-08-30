#ifndef POINT2D_VECJ7WQC
#define POINT2D_VECJ7WQC

/*
File: Point2D.h
Author: Samuel Barrett
Description: a 2-dimensional integer point
Created:  2011-08-22
Modified: 2011-08-22
*/

#include <ostream>

class Point2D {
public:
  Point2D();
  Point2D (int x, int y);

  bool operator==(const Point2D &other) const;
  bool operator!=(const Point2D &other) const;
  Point2D& operator+=(const Point2D &rhs);
  const Point2D operator+(const Point2D &rhs) const;
  Point2D& operator-=(const Point2D &rhs);
  const Point2D operator-(const Point2D &rhs) const;
  std::string toString() const;

public:
  int x;
  int y;
};

std::ostream& operator<<(std::ostream &out, const Point2D &pt);

#endif /* end of include guard: POINT2D_VECJ7WQC */
