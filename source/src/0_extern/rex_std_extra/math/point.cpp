// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: point.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std_extra/math/point.h"

rsl::Point::Point()
    : x(0)
    , y(0)
{
}
rsl::Point::Point(int16 _x, int16 _y)
    : x(_x)
    , y(_y)
{
}

rsl::Point rsl::Point::operator+(const Point& rhs) const
{
  return {x + rhs.x, y + rhs.y};
}
rsl::Point rsl::Point::operator-(const Point& rhs) const
{
  return {x - rhs.x, y - rhs.y};
}
rsl::Point rsl::Point::operator*(const Point& rhs) const
{
  return {x * rhs.x, y * rhs.y};
}
rsl::Point rsl::Point::operator/(const Point& rhs) const
{
  return {x / rhs.x, y / rhs.y};
}

rsl::Point& rsl::Point::operator+=(const Point& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}
rsl::Point& rsl::Point::operator-=(const Point& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}
rsl::Point& rsl::Point::operator*=(const Point& rhs)
{
  x *= rhs.x;
  y *= rhs.y;
  return *this;
}
rsl::Point& rsl::Point::operator/=(const Point& rhs)
{
  x /= rhs.x;
  y /= rhs.y;
  return *this;
}

rsl::Point rsl::Point::operator+(const int16 rhs) const
{
  return {x + rhs, y + rhs};
}

rsl::Point rsl::Point::operator-(const int16 rhs) const
{
  return {x - rhs, y - rhs};
}

rsl::Point rsl::Point::operator*(const int16 rhs) const
{
  return {x * rhs, y * rhs};
}

rsl::Point rsl::Point::operator/(const int16 rhs) const
{
  return {x / rhs, y / rhs};
}

rsl::Point& rsl::Point::operator+=(const int16 rhs)
{
  x += rhs;
  y += rhs;
  return *this;
}
rsl::Point& rsl::Point::operator-=(const int16 rhs)
{
  x -= rhs;
  y -= rhs;
  return *this;
}
rsl::Point& rsl::Point::operator*=(const int16 rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}
rsl::Point& rsl::Point::operator/=(const int16 rhs)
{
  x /= rhs;
  y /= rhs;
  return *this;
}

bool rsl::Point::operator==(const Point& point) const
{
  return x == point.x && y == point.y;
}
bool rsl::Point::operator!=(const Point& point) const
{
  return !(*this == point);
}

rsl::Point rsl::Point::operator-() const
{
  return {-x, -y};
}

rsl::ostream& rsl::operator<<(ostream& os, const Point& point)
{
  return os << "(x: " << point.x << ", y: " << point.y << ")";
}
