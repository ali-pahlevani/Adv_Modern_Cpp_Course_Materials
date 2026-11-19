// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 6.2 (Unit 6, slide 9)
//   ros2 run topic_monitor topic_monitor_specialized_node

#include <memory>

#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_msgs/msg/string.hpp>

#include "topic_monitor/topic_monitor_specialized.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("topic_monitor");

  TopicMonitor<sensor_msgs::msg::LaserScan> scan_monitor{*node, "scan"};   // three classes,
  TopicMonitor<nav_msgs::msg::Odometry> odom_monitor{*node, "odom"};       // one template
  TopicMonitor<std_msgs::msg::String> chatter_monitor{*node, "chatter"};

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
