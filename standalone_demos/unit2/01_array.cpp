// Unit 2, slides 6 and 7. std::array basics.
#include <array>
#include <iostream>
#include <stdexcept>
#include <string_view>

int main()
{
  // diffbot has exactly two wheels, and that's known at compile time
  std::array<std::string_view, 2> wheel_names{"left_wheel_joint", "right_wheel_joint"};
  std::array<double, 2> wheel_speeds_radps{4.85, 7.27};

  std::cout << wheel_names[1] << " spins at " << wheel_speeds_radps.at(1) << " rad/s\n";
  wheel_speeds_radps[1] = 8.00;
  std::cout << "after update: " << wheel_speeds_radps[1] << " rad/s\n";
  std::cout << "size is always " << wheel_speeds_radps.size() << '\n';

  // range-for: read with const&, modify with &
  for (double & w : wheel_speeds_radps) {w *= 2.0;}
  std::cout << "doubled:";
  for (const double w : wheel_speeds_radps) {std::cout << ' ' << w;}
  std::cout << '\n';

  // structured bindings
  const std::array<double, 3> position{0.0177, 0.00026, -0.00025};
  const auto [x, y, z] = position;
  std::cout << "x=" << x << " y=" << y << " z=" << z << '\n';

  // checked at compile time
  constexpr std::array<double, 2> kWheelRadii{0.033, 0.033};
  static_assert(kWheelRadii.size() == 2);
  static_assert(kWheelRadii[0] > 0.0);

  // at() throws instead of silently corrupting memory
  try {
    std::cout << wheel_speeds_radps.at(5) << '\n';
  } catch (const std::out_of_range &) {
    std::cout << "at(5) threw std::out_of_range\n";
  }
}
