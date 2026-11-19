// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// RobotManager after Exercises 3.1 to 3.6 (Unit 3, slides 13 to 36)

#include "my_robot_manager/robot_manager.hpp"

#include <utility>

RobotManager::RobotManager(
  rclcpp::Node & node, std::string robot_name, std::string robot_model,
  std::string odom_topic)
: robot_name_{std::move(robot_name)},
  robot_model_{std::move(robot_model)},
  odom_topic_{std::move(odom_topic)},
  logger_{node.get_logger()}
{
  odom_subscription_ = node.create_subscription<nav_msgs::msg::Odometry>(
    odom_topic_, 10,
    [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {odom_callback(*msg);});

  log_position_service_ = node.create_service<std_srvs::srv::Trigger>(
    robot_name_ + "/log_current_position",
    [this](
      const std::shared_ptr<std_srvs::srv::Trigger::Request>,
      std::shared_ptr<std_srvs::srv::Trigger::Response> response)
    {
      log_current_position(*response);
    });

  ++robot_count_;
  RCLCPP_INFO(logger_, "Robot %d created", robot_count_);
}

RobotManager::RobotManager(rclcpp::Node & node, const std::string & robot_name)
: RobotManager(node, robot_name, "diffbot", robot_name + "/odom")
{}

void RobotManager::print_specifications() const
{
  RCLCPP_INFO(logger_, "Robot name: %s", robot_name_.c_str());
  RCLCPP_INFO(logger_, "Robot model: %s", robot_model_.c_str());
  RCLCPP_INFO(logger_, "Battery capacity: %.2f kWh", battery_capacity_kwh_);
  RCLCPP_INFO(logger_, "Total operation hours: %d", total_operation_hours_);
}

void RobotManager::odom_callback(const nav_msgs::msg::Odometry & msg)
{
  current_x_m_ = msg.pose.pose.position.x;    // short for this->current_x_m_
  current_y_m_ = msg.pose.pose.position.y;
}

void RobotManager::log_current_position(std_srvs::srv::Trigger::Response & response) const
{
  RCLCPP_INFO(
    logger_, "%s position (x, y): %.3f, %.3f",
    robot_name_.c_str(), current_x_m_, current_y_m_);

  response.success = true;
  response.message = "Position logged";
}
