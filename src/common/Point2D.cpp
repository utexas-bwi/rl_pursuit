#include "Point2D.h"
#include <sstream>
#include <cstdlib>

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
  else if (x > other.x)
    return false;
  return y < other.y;
}

bool Point2D::operator==(const Point2D &other) const {
  return (x == other.x) && (y == other.y);
}

bool Point2D::operator!=(const Point2D &other) const {
  return (x != other.x) || (y != other.y);
}

Point2D Point2D::operator+(const Point2D &rhs) const {
  return Point2D(*this) += rhs;
}

Point2D& Point2D::operator+=(const Point2D &rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Point2D Point2D::operator-(const Point2D &rhs) const{
  return Point2D(*this) -= rhs;
}

Point2D& Point2D::operator-=(const Point2D &rhs) {
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Point2D Point2D::operator*(int factor) const {
  return Point2D(*this) *= factor;
}

Point2D& Point2D::operator*=(int factor) {
  x *= factor;
  y *= factor;
  return *this;
}

Point2D Point2D::operator*(float factor) const {
  return Point2D(*this) *= factor;
}

Point2D& Point2D::operator*=(float factor) {
  x *= factor;
  y *= factor;
  return *this;
}

std::string Point2D::toString() const{
  std::stringstream ss;
  ss << *this;
  return ss.str();
}

unsigned int Point2D::manhattanDist() const {
  return abs(x) + abs(y);
}

std::ostream& operator<<(std::ostream &out, const Point2D &pt) {
  out << "[" << pt.x << ", " << pt.y << "]";
  return out;
}

Point2D operator*(int factor,const Point2D &lhs) {
  return lhs * factor;
}

Point2D operator*(float factor,const Point2D &lhs) {
  return lhs * factor;
}
