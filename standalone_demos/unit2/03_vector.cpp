// Unit 2, slides 10 to 14. std::vector: creating, capacity, removing, and two common traps.
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

int main()
{
  // ---- slide 10: braces and parentheses mean different things ----
  const std::vector<int> x{5, 12};   // 2 elements: 5, 12
  const std::vector<int> y(5, 12);   // 5 elements: 12 12 12 12 12
  std::cout << "x{5, 12} has " << x.size() << " elements; y(5, 12) has " << y.size() << '\n';

  // ---- slide 12: size vs capacity ----
  std::vector<float> ranges;
  std::cout << "empty       : " << ranges.size() << " / " << ranges.capacity() << '\n';
  ranges.reserve(360);
  std::cout << "reserve(360): " << ranges.size() << " / " << ranges.capacity() << '\n';
  ranges.push_back(1.0f);
  std::cout << "push_back   : " << ranges.size() << " / " << ranges.capacity() << '\n';
  ranges.clear();
  std::cout << "clear()     : " << ranges.size() << " / " << ranges.capacity()
            << "   <- capacity kept\n";

  // ---- slide 13: the unsigned trap, and the C++20 fix ----
  const std::vector<float> empty;
  std::cout << "empty.size() - 1      = " << empty.size() - 1 << "   <- wrapped!\n";
  std::cout << "std::ssize(empty) - 1 = " << std::ssize(empty) - 1 << '\n';

  // ---- slide 14: std::erase_if (C++20) on a vector like LaserScan::ranges ----
  std::vector<float> scan{
    2.4f, 0.31f, std::numeric_limits<float>::infinity(), 1.05f, 0.87f, 12.0f};
  std::cout << "raw readings: " << scan.size() << '\n';

  const auto dropped = std::erase_if(scan, [](float r) {
    return !std::isfinite(r) || r > 10.0f;
  });
  std::cout << "dropped " << dropped << ", kept " << scan.size() << '\n';
  std::cout << "closest obstacle: " << *std::ranges::min_element(scan) << " m\n";
}
