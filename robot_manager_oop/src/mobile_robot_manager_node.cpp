// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercises 4.3 to 4.5 (Unit 4, slides 9 to 16)
//   ros2 run robot_manager_oop mobile_robot_manager_node
//   ros2 topic pub -r 2 /odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
//   ros2 service call /robot_manager_output std_srvs/srv/SetBool "{data: true}"

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "robot_manager_oop/mobile_robot_manager.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MobileRobotManager>());
  rclcpp::shutdown();
  return 0;
}
