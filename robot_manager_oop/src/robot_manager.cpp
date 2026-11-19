// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 4.1 (Unit 4, slides 3 to 5): a RobotManager that has a ComputerUnit

#include "robot_manager_oop/robot_manager.hpp"

#include <chrono>
#include <memory>

using SetBool = std_srvs::srv::SetBool;

RobotManager::RobotManager(rclcpp::Node & node)
: logger_{node.get_logger()}
{
  output_service_ = node.create_service<SetBool>(
    "robot_manager_output",
    [this](const std::shared_ptr<SetBool::Request> request,
    std::shared_ptr<SetBool::Response> response)
    {
      output_enabled_ = request->data;
      response->success = true;
      response->message = output_enabled_ ? "Console output enabled" : "Console output disabled";
      RCLCPP_INFO(logger_, "%s", response->message.c_str());
    });

  timer_ = node.create_wall_timer(std::chrono::seconds(1), [this]() {log_system_load();});
  RCLCPP_INFO(logger_, "Output service created");
}

void RobotManager::log_system_load()
{
  if (!output_enabled_) {
    return;
  }
  RCLCPP_INFO(
    logger_, "CPU: %.1f %%, memory: %.1f %%",
    computer_.cpu_usage_percent(), computer_.memory_usage_percent());
}
