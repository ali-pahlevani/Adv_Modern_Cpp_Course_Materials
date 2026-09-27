// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.7, the Unit 2 capstone (slides 54 to 57)
//   ros2 run exercises_unit_2 scan_analyser --ros-args -p safety_distance_m:=0.8
//
// Uses containers, iterators, algorithms, lambdas and ranges together.
// Rule for this exercise: no memory allocation in the callback, so every buffer
// is reserved once in the constructor.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iterator>
#include <memory>
#include <numeric>
#include <ranges>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

namespace
{
constexpr float kRadToDeg = 180.0f / 3.14159265f;
constexpr float kSectorHalfWidthRad = 30.0f / kRadToDeg;   // forward sector, 30 degrees each side
constexpr std::size_t kMaxRays = 2048;
constexpr std::ptrdiff_t kNearestCount = 5;
}  // namespace

struct Reading
{
  float range_m{0.0f};
  float bearing_rad{0.0f};
};

class ScanAnalyser : public rclcpp::Node
{
public:
  ScanAnalyser()
  : rclcpp::Node("scan_analyser")
  {
    safety_distance_m_ = declare_parameter<double>("safety_distance_m", 0.5);

    valid_.reserve(kMaxRays);        // allocate once, here
    scratch_.reserve(kMaxRays);

    subscription_ = create_subscription<sensor_msgs::msg::LaserScan>(
      "scan", rclcpp::SensorDataQoS(),
      [this](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {latest_ = msg;});

    timer_ = create_wall_timer(std::chrono::seconds(1), [this]() {report();});
  }

private:
  void report();

  double safety_distance_m_{0.5};
  std::vector<Reading> valid_;
  std::vector<float> scratch_;
  sensor_msgs::msg::LaserScan::ConstSharedPtr latest_;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};

void ScanAnalyser::report()
{
  if (!latest_) {
    RCLCPP_INFO(get_logger(), "waiting for the first /scan ...");
    return;
  }
  const auto & scan = *latest_;

  // 1. Valid readings with their bearings. clear() keeps the capacity: no allocation.
  valid_.clear();
  for (std::size_t i = 0; i < scan.ranges.size(); ++i) {
    const float r = scan.ranges[i];
    if (std::isfinite(r) && r >= scan.range_min && r <= scan.range_max) {
      valid_.push_back(Reading{
        .range_m = r,
        .bearing_rad = scan.angle_min + static_cast<float>(i) * scan.angle_increment});
    }
  }
  if (valid_.empty()) {
    RCLCPP_WARN(get_logger(), "no valid returns in this scan");
    return;
  }

  // 2. Closest obstacle, using a projection instead of a comparator lambda.
  //    Copied by value on purpose: step 5 reorders valid_, and an iterator would
  //    then point at whatever element moved into that slot.
  const Reading closest = *std::ranges::min_element(valid_, {}, &Reading::range_m);

  // 3. Mean and median of the ranges.
  scratch_.clear();
  std::ranges::transform(valid_, std::back_inserter(scratch_), &Reading::range_m);

  const double mean =
    std::accumulate(scratch_.begin(), scratch_.end(), 0.0) /   // 0.0, not 0
    static_cast<double>(scratch_.size());

  const auto mid = scratch_.begin() + std::ssize(scratch_) / 2;
  std::ranges::nth_element(scratch_, mid);                     // O(n), not O(n log n)
  const float median = *mid;

  // 4. Is anything in the forward sector closer than the safety distance?
  const bool sector_clear = std::ranges::none_of(
    valid_, [this](const Reading & reading) {
      return std::abs(reading.bearing_rad) <= kSectorHalfWidthRad &&
             reading.range_m < static_cast<float>(safety_distance_m_);
    });

  // 5. The nearest few. partial_sort is enough; no need to sort everything.
  const auto n = std::min(kNearestCount, std::ssize(valid_));
  std::ranges::partial_sort(valid_, valid_.begin() + n, {}, &Reading::range_m);

  RCLCPP_INFO(
    get_logger(),
    "valid %zu | closest %.2f m @ %.1f deg | mean %.2f | median %.2f | front %s",
    valid_.size(), closest.range_m, closest.bearing_rad * kRadToDeg,
    mean, median, sector_clear ? "CLEAR" : "BLOCKED");

  for (const Reading & reading : valid_ | std::views::take(n)) {
    RCLCPP_INFO(
      get_logger(), "   %.2f m @ %.1f deg",
      reading.range_m, reading.bearing_rad * kRadToDeg);
  }
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ScanAnalyser>());
  rclcpp::shutdown();
  return 0;
}
