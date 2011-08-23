#include "Point2D.h"

Point2D::Point2D():
  x(0),
  y(0)
{}

Point2D::Point2D(int x, int y):
  x(x),
  y(y)
{}

bool Point2D::operator==(const Point2D &other) const {
  return (x == other.x) && (y == other.y);
}

bool Point2D::operator!=(const Point2D &other) const {
  return (x != other.x) || (y != other.y);
}

Point2D& Point2D::operator+=(const Point2D &rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

const Point2D Point2D::operator+(const Point2D &rhs) {
  return Point2D(*this) += rhs;
}

std::ostream& operator<<(std::ostream &out, const Point2D &pt) {
  out << "(" << pt.x << "," << pt.y << ")";
  return out;
}
