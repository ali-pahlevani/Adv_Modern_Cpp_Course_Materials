// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// RobotManagerBase, Exercises 4.3 to 4.5 (Unit 4, slides 9 to 16)

#include "robot_manager_oop/robot_manager_base.hpp"

#include <chrono>
#include <memory>

using SetBool = std_srvs::srv::SetBool;

RobotManagerBase::RobotManagerBase(const std::string & node_name)
: rclcpp::Node{node_name}                                 // the base class is built first
{
  output_service_ = create_service<SetBool>(
    "robot_manager_output",
    [this](const std::shared_ptr<SetBool::Request> request,
    std::shared_ptr<SetBool::Response> response)
    {
      display_robot_details();                            // virtual: the derived version runs
      output_enabled_ = request->data;
      response->success = true;
      response->message = output_enabled_ ? "Console output enabled" : "Console output disabled";
      RCLCPP_INFO(get_logger(), "%s", response->message.c_str());
    });

  timer_ = create_wall_timer(std::chrono::seconds(1), [this]() {log_system_load();});
  RCLCPP_INFO(get_logger(), "Output service created");
}

void RobotManagerBase::display_robot_details() const
{
  RCLCPP_INFO(get_logger(), "Robot name: %s", robot_name_.c_str());
  RCLCPP_INFO(get_logger(), "Robot location: %s", robot_location_.c_str());
}

void RobotManagerBase::log_system_load()
{
  if (!output_enabled_) {
    return;
  }
  RCLCPP_INFO(
    get_logger(), "CPU: %.1f %%, memory: %.1f %%",
    computer_.cpu_usage_percent(), computer_.memory_usage_percent());
}
