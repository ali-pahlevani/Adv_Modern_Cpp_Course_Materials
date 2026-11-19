// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// The Unit 3 node, after Exercises 3.1 to 3.6
//   ros2 run my_robot_manager robot_manager_node
//   ros2 topic pub -r 2 /diffbot_1/odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
//   ros2 service call /diffbot_1/log_current_position std_srvs/srv/Trigger

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "my_robot_manager/robot_manager.hpp"

// A const reference: this function can only call const member functions (Exercise 3.4)
void log_summary(const RobotManager & manager, const rclcpp::Logger & logger)
{
  RCLCPP_INFO(
    logger, "Summary: %s has a %.2f kWh battery",
    manager.robot_name().c_str(), manager.battery_capacity_kwh());
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto node = std::make_shared<rclcpp::Node>("robot_manager");

  // Exercise 3.6: model "diffbot" and topic "<name>/odom" come from the delegating constructor
  RobotManager manager_1{*node, "diffbot_1"};
  manager_1.print_specifications();

  RobotManager manager_2{*node, "diffbot_2"};
  manager_2.print_specifications();

  RCLCPP_INFO(node->get_logger(), "Managing %d robots", RobotManager::robot_count());   // Exercise 3.3
  log_summary(manager_1, node->get_logger());
  log_summary(manager_2, node->get_logger());

  rclcpp::spin(node);      // runs callbacks until Ctrl+C
  rclcpp::shutdown();
  return 0;
}
