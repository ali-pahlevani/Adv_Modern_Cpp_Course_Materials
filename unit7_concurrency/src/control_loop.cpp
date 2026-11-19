// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Demo 7.3 (Unit 7, slide 5): executors and callback groups
//   ros2 run unit7_concurrency control_loop single     # then: multi, groups

#include <chrono>
#include <memory>
#include <string>
#include <thread>

#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

class ControlLoop : public rclcpp::Node
{
public:
  explicit ControlLoop(bool own_groups)
  : Node("control_loop")
  {
    if (own_groups) {                                    // otherwise nullptr: the default group
      control_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
      planner_group_ = create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);
    }
    control_timer_ = create_wall_timer(100ms, [this] {on_control();}, control_group_);
    planner_timer_ = create_wall_timer(1s, [this] {on_planner();}, planner_group_);
  }

private:
  void on_control()                                      // must run every 100 ms
  {
    const auto now = std::chrono::steady_clock::now();
    const auto gap = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_);
    if (gap > 150ms) {
      RCLCPP_WARN(get_logger(), "control late: %ld ms since the last run", gap.count());
    }
    last_ = now;
  }

  void on_planner()                                      // slow: 400 ms of work
  {
    RCLCPP_INFO(get_logger(), "planner start");
    std::this_thread::sleep_for(400ms);
  }

  std::chrono::steady_clock::time_point last_{std::chrono::steady_clock::now()};
  rclcpp::CallbackGroup::SharedPtr control_group_;       // members: the node only keeps a weak_ptr
  rclcpp::CallbackGroup::SharedPtr planner_group_;
  rclcpp::TimerBase::SharedPtr control_timer_;
  rclcpp::TimerBase::SharedPtr planner_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  const std::string mode = argc > 1 ? argv[1] : "single";
  auto node = std::make_shared<ControlLoop>(mode == "groups");

  if (mode == "single") {
    rclcpp::spin(node);                                  // SingleThreadedExecutor: one thread
  } else {
    rclcpp::executors::MultiThreadedExecutor executor(rclcpp::ExecutorOptions(), 2);
    executor.add_node(node);
    executor.spin();                                     // two threads
  }
  rclcpp::shutdown();
}
