// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 4.1 (Unit 4, slide 3)
//   ros2 run robot_manager_oop robot_manager_node
//   ros2 service call /robot_manager_output std_srvs/srv/SetBool "{data: true}"

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "robot_manager_oop/robot_manager.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("robot_manager");
  RobotManager manager{*node};
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
