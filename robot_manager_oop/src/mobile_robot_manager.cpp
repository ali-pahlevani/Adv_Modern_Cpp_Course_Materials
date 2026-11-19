// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// MobileRobotManager, Exercises 4.3 and 4.4 (Unit 4, slides 9 to 14)

#include "robot_manager_oop/mobile_robot_manager.hpp"

MobileRobotManager::MobileRobotManager()
: RobotManagerBase{"mobile_robot_manager"}                // call the base constructor
{
  odom_subscription_ = create_subscription<nav_msgs::msg::Odometry>(
    "odom", 10,
    [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {odom_callback(*msg);});
  RCLCPP_INFO(get_logger(), "Odometry subscriber created");
}

void MobileRobotManager::display_robot_details() const
{
  RobotManagerBase::display_robot_details();              // the base part first
  RCLCPP_INFO(get_logger(), "Battery charge level: %.2f", battery_charge_level_);
  RCLCPP_INFO(get_logger(), "Battery type: %s", battery_type_.c_str());
}

void MobileRobotManager::odom_callback(const nav_msgs::msg::Odometry & msg) const
{
  if (output_enabled_) {                                  // a protected base member
    RCLCPP_INFO(
      get_logger(), "Position (x, y): %.3f, %.3f",
      msg.pose.pose.position.x, msg.pose.pose.position.y);
  }
}
