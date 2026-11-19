// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 5.3 (Unit 5, slide 9): smart pointers only, no destructor
//   ros2 run laser_reader laser_reader
//   ros2 topic pub -r 2 /scan sensor_msgs/msg/LaserScan "{angle_min: -1.5708, angle_increment: 0.7854, range_min: 0.1, range_max: 10.0, ranges: [1.0, 1.5, 2.0, 2.5]}"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <memory>
#include <numbers>
#include <optional>
#include <utility>

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

using sensor_msgs::msg::LaserScan;

// The ray at angle 0 points straight ahead
std::size_t front_index(const LaserScan & scan, std::size_t count)
{
  const long index = std::lround(-scan.angle_min / scan.angle_increment);
  return static_cast<std::size_t>(std::clamp(index, 0L, static_cast<long>(count) - 1));
}

struct Obstacle
{
  float range_m{0.0f};
  float angle_rad{0.0f};
};

// const &: reads the whole scan without copying it, and promises not to change it
std::optional<Obstacle> find_closest_obstacle(const LaserScan & scan)
{
  std::optional<Obstacle> closest;                          // empty: nothing found yet
  for (std::size_t i = 0; i < scan.ranges.size(); ++i) {
    const float range = scan.ranges[i];
    const bool valid = std::isfinite(range) && range >= scan.range_min && range <= scan.range_max;
    if (valid && (!closest || range < closest->range_m)) {
      closest = Obstacle{range, scan.angle_min + static_cast<float>(i) * scan.angle_increment};
    }
  }
  return closest;
}

class LaserReader : public rclcpp::Node
{
public:
  LaserReader()
  : rclcpp::Node{"laser_reader"},
    last_ranges_{std::make_unique<float[]>(kMaxRays)}       // zeroed, freed automatically
  {
    for (auto & scan : last_ten_scans_) {
      scan = std::make_unique<float[]>(kMaxRays);
    }
    subscription_ = create_subscription<LaserScan>(
      "scan", rclcpp::SensorDataQoS(),
      [this](LaserScan::ConstSharedPtr msg) {on_scan(std::move(msg));});
    timer_ = create_wall_timer(std::chrono::seconds(1), [this]() {report();});
  }

  // No destructor: the unique_ptrs free every buffer (rule of zero)

private:
  static constexpr std::size_t kMaxRays{1024};
  static constexpr std::size_t kHistory{10};

  void on_scan(LaserScan::ConstSharedPtr msg)
  {
    const LaserScan & scan = *msg;
    const std::size_t count = std::min(scan.ranges.size(), kMaxRays);   // never write past the end
    if (count == 0 || scan.angle_increment == 0.0f) {
      return;
    }
    std::copy_n(scan.ranges.begin(), count, last_ranges_.get());
    std::copy_n(scan.ranges.begin(), count, last_ten_scans_[next_slot_].get());
    next_slot_ = (next_slot_ + 1) % kHistory;               // a ring: overwrite the oldest next
    stored_ = std::min(stored_ + 1, kHistory);

    const std::size_t front = front_index(scan, count);
    float sum = 0.0f;
    for (std::size_t s = 0; s < stored_; ++s) {
      sum += last_ten_scans_[s][front];                     // array of pointers: two indexes
    }
    RCLCPP_INFO(
      get_logger(), "front: %.2f m, average of the last %zu scans: %.2f m",
      last_ranges_[front], stored_, sum / static_cast<float>(stored_));
    latest_scan_ = std::move(msg);                          // keep the message alive: no copy
  }

  void report() const                                       // once a second, from the timer
  {
    if (!latest_scan_) {
      return;                                               // no scan yet
    }
    if (const auto obstacle = find_closest_obstacle(*latest_scan_)) {
      RCLCPP_INFO(
        get_logger(), "closest obstacle: %.2f m at %.0f deg",
        obstacle->range_m, obstacle->angle_rad * 180.0f / std::numbers::pi_v<float>);
    }
  }

  std::unique_ptr<float[]> last_ranges_;
  std::array<std::unique_ptr<float[]>, kHistory> last_ten_scans_;
  LaserScan::ConstSharedPtr latest_scan_;                   // shares ownership of the message
  std::size_t next_slot_{0};
  std::size_t stored_{0};
  rclcpp::Subscription<LaserScan>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LaserReader>());
  rclcpp::shutdown();
  return 0;
}
