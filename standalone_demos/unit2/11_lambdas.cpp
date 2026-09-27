// Unit 2, slides 45 to 49. Lambdas and function objects.
#include <algorithm>
#include <cmath>
#include <concepts>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

// A function object written by hand. A lambda generates something like this for you.
struct IsTooClose
{
  float threshold_m;
  bool operator()(float r) const {return r < threshold_m;}
};

// Returns a lambda set up at runtime. It captures by value, so returning it is safe.
auto make_obstacle_filter(float min_m, float max_m)
{
  return [min_m, max_m](float r) {
           return std::isfinite(r) && r >= min_m && r <= max_m;
         };
}

int main()
{
  const std::vector<float> ranges{
    0.05f, 0.31f, std::numeric_limits<float>::infinity(), 1.05f, 0.87f, 12.0f};

  // ---- slide 45: function object vs lambda ----
  std::cout << "struct : " << std::ranges::count_if(ranges, IsTooClose{0.5f}) << " too close\n";
  std::cout << "lambda : "
            << std::ranges::count_if(ranges, [](float r) {return r < 0.5f;}) << " too close\n";

  // ---- slide 49: one configured lambda, reused ----
  const auto is_obstacle = make_obstacle_filter(0.12f, 3.5f);
  std::cout << std::ranges::count_if(ranges, is_obstacle) << " obstacle returns\n";

  std::vector<float> obstacles;
  obstacles.reserve(ranges.size());
  std::ranges::copy_if(ranges, std::back_inserter(obstacles), is_obstacle);
  std::ranges::sort(obstacles);
  std::cout << "closest: " << obstacles.front() << " m\n";

  // ---- slide 47: mutable state, generic and constrained lambdas ----
  auto counter = [n = 0]() mutable {return ++n;};
  counter();
  counter();
  std::cout << "counter called 3 times -> " << counter() << '\n';

  constexpr auto square = [](int x) {return x * x;};
  static_assert(square(4) == 16);

  const auto print = [](const auto & x) {std::cout << x << ' ';};
  print(42);
  print(3.14);
  print("odom");
  std::cout << '\n';

  const auto scale = []<std::floating_point T>(T x, T k) {return x * k;};
  std::cout << "scale(1.5, 2.0) = " << scale(1.5, 2.0) << '\n';
  // scale(1, 2);   // won't compile: int is not a floating_point type

  // ---- standard function objects ----
  std::vector<int> v{3, 1, 2};
  std::ranges::sort(v, std::greater{});
  std::cout << "sorted with std::greater{}:";
  for (const int x : v) {std::cout << ' ' << x;}
  std::cout << '\n';
}
