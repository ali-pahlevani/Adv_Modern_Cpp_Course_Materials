// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// MobileRobotManager, Exercises 4.3 and 4.4 (Unit 4, slides 9 to 14)

#ifndef ROBOT_MANAGER_OOP__MOBILE_ROBOT_MANAGER_HPP_
#define ROBOT_MANAGER_OOP__MOBILE_ROBOT_MANAGER_HPP_

#include <string>

#include <nav_msgs/msg/odometry.hpp>

#include "robot_manager_oop/robot_manager_base.hpp"

class MobileRobotManager : public RobotManagerBase        // is-a robot manager
{
public:
  MobileRobotManager();

protected:
  void display_robot_details() const override;            // Exercise 4.4

private:
  void odom_callback(const nav_msgs::msg::Odometry & msg) const;

  double battery_charge_level_{0.55};
  std::string battery_type_{"Li-ion"};
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscription_;
};

#endif  // ROBOT_MANAGER_OOP__MOBILE_ROBOT_MANAGER_HPP_
