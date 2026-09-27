// Unit 2, slides 50 to 52. C++20 ranges and views.
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <ranges>
#include <string>
#include <vector>
#include <version>

int main()
{
  const std::vector<float> raw{
    2.4f, 0.31f, std::numeric_limits<float>::infinity(),
    1.05f, 0.87f, 12.0f, 0.55f, 0.42f};

  // ---- slide 50: a lazy pipeline; raw is never copied or modified ----
  auto valid = raw | std::views::filter([](float r) {
      return std::isfinite(r) && r <= 10.0f;
    });

  std::cout << "valid readings : ";
  for (const float r : valid) {std::cout << r << ' ';}

  // Round, don't truncate: 1.05f is really 1.0499999..., so a bare cast gives 104.
  std::cout << "\nin centimetres : ";
  for (const long cm : valid | std::views::transform(
      [](float r) {return std::lround(r * 100.0f);}))
  {
    std::cout << cm << ' ';
  }

  std::cout << "\nfirst 3        : ";
  for (const float r : valid | std::views::take(3)) {std::cout << r << ' ';}
  std::cout << '\n';

  // ---- slide 51: copy a view into a vector, so the allocation is visible ----
  // Named lambda and view (slide 53). Easier to read, and it also avoids a Clang 18
  // problem with lambdas written inline inside a function call's arguments.
  const auto under_one_metre = [](float r) {return r < 1.0f;};
  auto close_view = valid | std::views::filter(under_one_metre);

  std::vector<float> close;
  close.reserve(raw.size());
  std::ranges::copy(close_view, std::back_inserter(close));
  std::cout << "materialised " << close.size() << " readings under 1 m\n";

  // ---- views over a map ----
  const std::map<std::string, double> joints{
    {"left_wheel_joint", 1.57}, {"right_wheel_joint", 3.14}, {"caster_joint", 0.0}};
  std::cout << "joints above 1 rad: ";
  for (const auto & name : joints
    | std::views::filter([](const auto & kv) {return kv.second > 1.0;})
    | std::views::keys)
  {
    std::cout << name << ' ';
  }
  std::cout << '\n';

  // ---- slide 52: index + value ----
#if defined(__cpp_lib_ranges_enumerate)
  std::cout << "views::enumerate (C++23, Jazzy with -std=c++23):\n";
  for (const auto & [i, r] : raw | std::views::enumerate) {
    std::cout << "  " << i << ": " << r << '\n';
  }
#else
  std::cout << "views::iota stand-in (C++20, works everywhere):\n";
  for (const auto i : std::views::iota(std::size_t{0}, raw.size())) {
    std::cout << "  " << i << ": " << raw[i] << '\n';
  }
#endif
}
