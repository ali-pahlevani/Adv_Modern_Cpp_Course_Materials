// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// RobotManager after Exercises 3.1 to 3.6 (Unit 3)

#ifndef MY_ROBOT_MANAGER__ROBOT_MANAGER_HPP_
#define MY_ROBOT_MANAGER__ROBOT_MANAGER_HPP_

#include <string>

#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <std_srvs/srv/trigger.hpp>

class RobotManager
{
public:
  RobotManager(
    rclcpp::Node & node, std::string robot_name, std::string robot_model,
    std::string odom_topic = "odom");

  // Delegating constructor: model "diffbot", odometry on "<robot_name>/odom" (Exercise 3.6)
  RobotManager(rclcpp::Node & node, const std::string & robot_name);

  // The callbacks captured `this`. A copy would still point at the original object.
  RobotManager(const RobotManager &) = delete;
  RobotManager & operator=(const RobotManager &) = delete;

  void print_specifications() const;

  const std::string & robot_name() const {return robot_name_;}
  double battery_capacity_kwh() const {return battery_capacity_kwh_;}

  static int robot_count() {return robot_count_;}

private:
  void odom_callback(const nav_msgs::msg::Odometry & msg);   // not const: it stores the position
  void log_current_position(std_srvs::srv::Trigger::Response & response) const;

  inline static int robot_count_{0};   // one counter, shared by every RobotManager

  std::string robot_name_;
  std::string robot_model_;
  double battery_capacity_kwh_{0.16};
  int total_operation_hours_{2560};
  std::string odom_topic_;
  rclcpp::Logger logger_;
  double current_x_m_{0.0};
  double current_y_m_{0.0};

  // Declared last, so they are destroyed first: no callback can run on a half-destroyed object
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscription_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr log_position_service_;
};

#endif  // MY_ROBOT_MANAGER__ROBOT_MANAGER_HPP_
