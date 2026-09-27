// Unit 2, slide 8. One function that accepts any contiguous container, via std::span.
#include <array>
#include <iostream>
#include <span>
#include <vector>

double mean(std::span<const double> values)
{
  if (values.empty()) {return 0.0;}
  double sum = 0.0;
  for (const double v : values) {sum += v;}
  return sum / static_cast<double>(values.size());
}

int main()
{
  const std::array<double, 3> a{1.0, 2.0, 3.0};
  const std::vector<double> v{1.0, 2.0, 3.0, 10.0};
  const double raw[3]{1.0, 2.0, 3.0};

  std::cout << "array  : " << mean(a) << '\n';
  std::cout << "vector : " << mean(v) << '\n';
  std::cout << "C array: " << mean(raw) << '\n';
  std::cout << "first 2 of the vector: " << mean(std::span{v}.first(2)) << '\n';
  std::cout << "last 2 of the vector : " << mean(std::span{v}.last(2)) << '\n';
}
