#include <iostream>

#include "demo_63_moving_average.hpp"

int main()
{
  MovingAverage<float, 3> front_range;               // float, window of 3
  for (const float range : {1.00f, 1.20f, 0.20f, 1.10f, 1.00f}) {   // 0.20 is a noise spike
    front_range.add(range);
    std::cout << range << " -> " << front_range.value() << '\n';
  }

  MovingAverage<double, 50> wheel_speed;             // another type and size: another class
  wheel_speed.add(4.85);
  std::cout << "wheel speed: " << wheel_speed.value() << '\n';
}
