// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 4.1 (Unit 4, slides 3 to 5): a RobotManager that has a ComputerUnit

#ifndef ROBOT_MANAGER_OOP__ROBOT_MANAGER_HPP_
#define ROBOT_MANAGER_OOP__ROBOT_MANAGER_HPP_

#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/set_bool.hpp>

#include "robot_manager_oop/computer_unit.hpp"

class RobotManager
{
public:
  explicit RobotManager(rclcpp::Node & node);
  RobotManager(const RobotManager &) = delete;              // callbacks capture this (Unit 3)
  RobotManager & operator=(const RobotManager &) = delete;

private:
  void log_system_load();

  rclcpp::Logger logger_;
  ComputerUnit computer_;                                   // has-a: composition
  bool output_enabled_{false};
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr output_service_;
  rclcpp::TimerBase::SharedPtr timer_;
};

#endif  // ROBOT_MANAGER_OOP__ROBOT_MANAGER_HPP_
