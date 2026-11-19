#include <iostream>
#include <stdexcept>
#include <vector>

double turn_radius(double v, double w)               // R = v / w
{
  if (w == 0.0) {
    throw std::invalid_argument("w is 0: the robot drives straight");
  }
  return v / w;
}

struct Motor
{
  ~Motor() {std::cout << "  motor stopped\n";}       // runs even when an exception passes
};

void drive(double v, double w)
{
  Motor motor;
  const double radius = turn_radius(v, w);
  std::cout << "  radius: " << radius << " m\n";     // skipped when turn_radius throws
}

int main()
{
  std::cout << "1.0 / 0.0 = " << 1.0 / 0.0 << '\n';  // no exception: inf

  for (const double w : {0.5, 0.0}) {
    std::cout << "w = " << w << '\n';
    try {
      drive(0.1485, w);
    } catch (const std::invalid_argument & e) {      // most specific type first
      std::cout << "  invalid_argument: " << e.what() << '\n';
    } catch (const std::exception & e) {             // any standard exception
      std::cout << "  exception: " << e.what() << '\n';
    } catch (...) {                                  // anything else
      std::cout << "  unknown exception\n";
    }
  }

  std::vector<float> ranges(360);
  try {
    std::cout << ranges.at(400) << '\n';             // at() checks the index, [] does not
  } catch (const std::out_of_range & e) {
    std::cout << "out_of_range: " << e.what() << '\n';
  }
}
