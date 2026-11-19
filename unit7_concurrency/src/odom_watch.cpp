// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 7.2 (Unit 7, slide 6): two callback groups, two threads and a mutex
//   ros2 topic pub -r 20 /odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
//   ros2 run unit7_concurrency odom_watch

#include <algorithm>
#include <chrono>
#include <memory>
#include <mutex>
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
    odom_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    report_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

    rclcpp::SubscriptionOptions options;
    options.callback_group = odom_group_;
    odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(
      "odom", 10, [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {on_odom(*msg);}, options);
    report_timer_ = create_wall_timer(1s, [this] {on_report();}, report_group_);
  }

private:
  void on_odom(const nav_msgs::msg::Odometry & msg)
  {
    const auto now = std::chrono::steady_clock::now();
    std::lock_guard lock(mutex_);                        // the report thread reads these too
    if (count_ > 0) {max_gap_ = std::max(max_gap_, now - last_odom_);}
    last_odom_ = now;
    x_ = msg.pose.pose.position.x;
    ++count_;
  }

  void on_report()
  {
    double x{};
    int count{};
    std::chrono::milliseconds gap{};
    {
      std::lock_guard lock(mutex_);                      // copy out under the lock...
      x = x_;
      count = count_;
      gap = std::chrono::duration_cast<std::chrono::milliseconds>(max_gap_);
      max_gap_ = {};
    }                                                    // ...then log and do slow work unlocked
    RCLCPP_INFO(get_logger(), "x=%.2f, %d msgs so far, max gap %ld ms", x, count, gap.count());
    std::this_thread::sleep_for(300ms);
  }

  std::mutex mutex_;
  double x_{0.0};
  int count_{0};
  std::chrono::steady_clock::time_point last_odom_;
  std::chrono::steady_clock::duration max_gap_{};

  rclcpp::CallbackGroup::SharedPtr odom_group_;          // members: the node does not own them
  rclcpp::CallbackGroup::SharedPtr report_group_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;
  rclcpp::TimerBase::SharedPtr report_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<OdomWatch>();
  rclcpp::executors::MultiThreadedExecutor executor(rclcpp::ExecutorOptions(), 2);
  executor.add_node(node);
  executor.spin();
  rclcpp::shutdown();
}
