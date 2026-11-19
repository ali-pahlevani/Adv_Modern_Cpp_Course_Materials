#include <iostream>

struct Vector2
{
  double x{0.0};
  double y{0.0};

  Vector2 operator+(const Vector2 & other) const {return {x + other.x, y + other.y};}
  Vector2 operator*(double k) const {return {x * k, y * k};}
  bool operator==(const Vector2 &) const = default;            // C++20: compares every member
};

std::ostream & operator<<(std::ostream & os, const Vector2 & v)  // a free function
{
  return os << '(' << v.x << ", " << v.y << ')';
}

int main()
{
  const Vector2 start{1.0, 2.0};
  const Vector2 velocity{0.2, 0.1};
  const Vector2 after_5s = start + velocity * 5.0;             // reads like the maths

  std::cout << "after 5 s: " << after_5s << '\n';
  std::cout << std::boolalpha << "back at start? " << (after_5s == start) << '\n';
}
