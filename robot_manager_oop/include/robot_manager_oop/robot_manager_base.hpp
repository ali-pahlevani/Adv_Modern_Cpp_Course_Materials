// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// RobotManagerBase, Exercises 4.3 to 4.5 (Unit 4, slides 9 to 16)
// It is a node, has a ComputerUnit, and is abstract since Exercise 4.5.

#ifndef ROBOT_MANAGER_OOP__ROBOT_MANAGER_BASE_HPP_
#define ROBOT_MANAGER_OOP__ROBOT_MANAGER_BASE_HPP_

#include <string>

#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/set_bool.hpp>

#include "robot_manager_oop/computer_unit.hpp"

class RobotManagerBase : public rclcpp::Node              // is-a node
{
public:
  explicit RobotManagerBase(const std::string & node_name);

protected:
  virtual void display_robot_details() const = 0;        // pure virtual: Exercise 4.5

  bool output_enabled_{false};                            // derived classes may read these
  std::string robot_name_{"diffbot_1"};
  std::string robot_location_{"Robotics lab"};

private:
  void log_system_load();

  ComputerUnit computer_;                                 // still has-a ComputerUnit
  rclcpp::Service<std_srvs::srv::SetBool>::SharedPtr output_service_;
  rclcpp::TimerBase::SharedPtr timer_;
};

#endif  // ROBOT_MANAGER_OOP__ROBOT_MANAGER_BASE_HPP_
