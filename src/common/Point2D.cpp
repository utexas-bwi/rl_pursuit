#include "Point2D.h"
#include <sstream>

Point2D::Point2D():
  x(0),
  y(0)
{}

Point2D::Point2D(int x, int y):
  x(x),
  y(y)
{}

bool Point2D::operator<(const Point2D &other) const {
  if (x < other.x)
    return true;
  return y < other.y;
}

bool Point2D::operator==(const Point2D &other) const {
  return (x == other.x) && (y == other.y);
}

bool Point2D::operator!=(const Point2D &other) const {
  return (x != other.x) || (y != other.y);
}

const Point2D Point2D::operator+(const Point2D &rhs) const {
  return Point2D(*this) += rhs;
}

Point2D& Point2D::operator+=(const Point2D &rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

const Point2D Point2D::operator-(const Point2D &rhs) const{
  return Point2D(*this) -= rhs;
}

Point2D& Point2D::operator-=(const Point2D &rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

std::string Point2D::toString() const{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

std::ostream& operator<<(std::ostream &out, const Point2D &pt) {
  out << "(" << pt.x << "," << pt.y << ")";
  return out;
}
