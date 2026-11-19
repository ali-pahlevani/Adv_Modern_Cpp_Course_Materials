// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 7.2, step 3 (Unit 7, slide 6): the version that stalls
// One thread, no callback groups, no mutex. Compare with odom_watch.cpp.

#include <algorithm>
#include <chrono>
#include <memory>
#include <thread>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class OdomWatch : public rclcpp::Node
{
public:
  OdomWatch()
  : Node("odom_watch")
  {
    odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
      "odom", 10, [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {on_odom(*msg);});
    report_timer_ = create_wall_timer(1s, [this] {on_report();});
  }

private:
  void on_odom(const nav_msgs::msg::Odometry & msg)
  {
    const auto now = std::chrono::steady_clock::now();
    if (count_ > 0) {max_gap_ = std::max(max_gap_, now - last_odom_);}
    last_odom_ = now;
    x_ = msg.pose.pose.position.x;
    ++count_;
  }

  void on_report()
  {
    const auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(max_gap_);
    RCLCPP_INFO(get_logger(), "x=%.2f, %d msgs so far, max gap %ld ms", x_, count_, gap.count());
    max_gap_ = {};
    std::this_thread::sleep_for(300ms);                  // slow: writes a log file
  }

  double x_{0.0};
  int count_{0};
  std::chrono::steady_clock::time_point last_odom_;
  std::chrono::steady_clock::duration max_gap_{};
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp::TimerBase::SharedPtr report_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<OdomWatch>());
  rclcpp::shutdown();
}
