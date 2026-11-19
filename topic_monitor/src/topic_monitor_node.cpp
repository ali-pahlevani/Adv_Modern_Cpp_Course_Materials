// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercises 6.1 to 6.3 (Unit 6, slides 6 to 14)
//   ros2 run topic_monitor topic_monitor_node

#include <memory>

#include <geometry_msgs/msg/pose_with_covariance_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>
#include <std_msgs/msg/string.hpp>

#include "topic_monitor/topic_monitor.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("topic_monitor");

  TopicMonitor<sensor_msgs::msg::LaserScan> scan_monitor{*node, "scan"};   // three classes,
  TopicMonitor<nav_msgs::msg::Odometry> odom_monitor{*node, "odom"};       // one template
  TopicMonitor<std_msgs::msg::String> chatter_monitor{*node, "chatter"};
  TopicMonitor<geometry_msgs::msg::PoseWithCovarianceStamped> pose_monitor{*node, "amcl_pose"};

  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
