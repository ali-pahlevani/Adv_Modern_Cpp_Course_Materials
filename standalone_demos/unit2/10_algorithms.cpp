// Unit 2, slides 37 to 43. Standard algorithms instead of hand-written loops.
#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>

struct Reading
{
  float range_m{};
  float angle_rad{};
};

int main()
{
  const std::vector<float> scan{
    2.4f, 0.31f, std::numeric_limits<float>::infinity(), 1.05f, 0.87f, 12.0f, 0.55f};

  // ---- slide 38: asking questions ----
  const auto is_valid = [](float r) {return std::isfinite(r) && r <= 10.0f;};
  std::cout << "valid readings : " << std::ranges::count_if(scan, is_valid) << '\n';
  std::cout << "all valid?     : " << std::boolalpha << std::ranges::all_of(scan, is_valid) << '\n';
  std::cout << "any < 0.5 m?   : "
            << std::ranges::any_of(scan, [](float r) {return r < 0.5f;}) << '\n';

  std::vector<float> valid;
  valid.reserve(scan.size());
  std::ranges::copy_if(scan, std::back_inserter(valid), is_valid);

  const auto [lo, hi] = std::ranges::minmax_element(valid);
  std::cout << "closest / furthest: " << *lo << " / " << *hi << " m\n";

  // ---- slide 39: sorting, projections, O(n) median ----
  std::vector<Reading> readings{{2.4f, 0.0f}, {0.31f, 1.2f}, {1.05f, -0.5f}, {0.87f, 2.9f}};
  std::ranges::sort(readings, {}, &Reading::range_m);   // project onto the member
  std::cout << "sorted by range:";
  for (const Reading & r : readings) {std::cout << ' ' << r.range_m;}
  std::cout << '\n';

  std::vector<float> for_median = valid;
  const auto mid = for_median.begin() + std::ssize(for_median) / 2;
  std::ranges::nth_element(for_median, mid);
  std::cout << "median (nth_element, O(n)): " << *mid << '\n';

  // ---- slide 40: remove_if doesn't actually remove anything; erase does ----
  std::vector<float> v{1.0f, -1.0f, 2.0f, -2.0f};
  const auto tail = std::ranges::remove_if(v, [](float x) {return x < 0.0f;});
  std::cout << "after remove_if, size() is still " << v.size() << '\n';
  v.erase(tail.begin(), tail.end());
  std::cout << "after erase, size() is " << v.size() << '\n';

  // ---- slide 41: the accumulate bug ----
  const std::vector<double> d{1.5, 2.5, 3.5};
  std::cout << "accumulate(..., 0)   = " << std::accumulate(d.begin(), d.end(), 0)
            << "   <- int seed truncates every step\n";
  std::cout << "accumulate(..., 0.0) = " << std::accumulate(d.begin(), d.end(), 0.0) << '\n';

  const double mean = std::accumulate(valid.begin(), valid.end(), 0.0) / std::ssize(valid);
  const double sq = std::accumulate(valid.begin(), valid.end(), 0.0,
      [mean](double acc, double x) {return acc + (x - mean) * (x - mean);});
  std::cout << "mean " << mean << ", sample stddev " << std::sqrt(sq / (std::ssize(valid) - 1))
            << '\n';

  // ---- de-duplicate: sort, unique, erase ----
  std::vector<int> ids{3, 1, 3, 2, 1};
  std::ranges::sort(ids);
  const auto dup = std::ranges::unique(ids);
  ids.erase(dup.begin(), dup.end());
  std::cout << "unique ids:";
  for (const int id : ids) {std::cout << ' ' << id;}
  std::cout << '\n';
}
