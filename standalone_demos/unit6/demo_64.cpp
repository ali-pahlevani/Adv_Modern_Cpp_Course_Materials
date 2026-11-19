#include <concepts>
#include <iostream>
#include <vector>

template <typename T>                                  // unconstrained: any T is accepted
T mean(const std::vector<T> & values)
{
  T sum{};
  for (const T & v : values) {sum += v;}
  return sum / static_cast<T>(values.size());
}

template <std::floating_point T>                      // constrained: float, double, long double
T safe_mean(const std::vector<T> & values)
{
  T sum{};
  for (const T & v : values) {sum += v;}
  return sum / static_cast<T>(values.size());
}

int main()
{
  const std::vector<int> ticks{1, 2};
  const std::vector<double> ranges_m{1.0, 2.0};
  std::cout << "mean(ticks)         = " << mean(ticks) << "   <- integer division!\n";
  std::cout << "safe_mean(ranges_m) = " << safe_mean(ranges_m) << '\n';
  // safe_mean(ticks);                                // rejected at the call site
}
