#include <expected>
#include <iostream>
#include <string>

std::expected<double, std::string> turn_radius(double v, double w)   // C++23
{
  if (w == 0.0) {
    return std::unexpected("w is 0: the robot drives straight");
  }
  return v / w;
}

int main()
{
  for (const double w : {0.5, 0.0}) {
    const auto radius = turn_radius(0.1485, w);
    if (radius) {
      std::cout << "radius: " << *radius << " m\n";
    } else {
      std::cout << "error: " << radius.error() << '\n';
    }
  }
}
