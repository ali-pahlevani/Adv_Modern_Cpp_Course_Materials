// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 6.2 (Unit 6, slide 9): describe() with full specializations
// Kept to compare with topic_monitor.hpp, the Exercise 6.3 version.

#pragma once

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>
#include <utility>

#include <rclcpp/rclcpp.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rosidl_runtime_cpp/traits.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

template <typename MessageT>                          // any ROS 2 message type
class TopicMonitor
{
public:
  TopicMonitor(rclcpp::Node & node, std::string topic)
  : logger_{node.get_logger()}, topic_{std::move(topic)}
  {
    subscription_ = node.create_subscription<MessageT>(
      topic_, rclcpp::SensorDataQoS(),
      [this](typename MessageT::ConstSharedPtr msg) {on_message(*msg);});
    RCLCPP_INFO(
      logger_, "Monitoring '%s' (%s)", topic_.c_str(), rosidl_generator_traits::name<MessageT>());
  }

  TopicMonitor(const TopicMonitor &) = delete;             // the callback captures this (Unit 3)
  TopicMonitor & operator=(const TopicMonitor &) = delete;

private:
  void on_message(const MessageT & msg)
  {
    ++count_;
    RCLCPP_INFO(logger_, "[%s #%zu] %s", topic_.c_str(), count_, describe(msg).c_str());
  }

  std::string describe(const MessageT & msg) const;        // defined below, per type

  rclcpp::Logger logger_;
  std::string topic_;
  std::size_t count_{0};
  typename rclcpp::Subscription<MessageT>::SharedPtr subscription_;   // depends on MessageT: typename
};

// Primary template: any type without a specialization. to_yaml() is generated for every message.
template <typename MessageT>
std::string TopicMonitor<MessageT>::describe(const MessageT & msg) const
{
  return to_yaml(msg, true);                                // found by argument-dependent lookup
}

// Full specializations: after the primary, at namespace scope, and inline (this is a header)
template <>
inline std::string TopicMonitor<sensor_msgs::msg::LaserScan>::describe(
  const sensor_msgs::msg::LaserScan & scan) const
{
  float closest = scan.range_max;
  for (const float range : scan.ranges) {
    if (std::isfinite(range) && range >= scan.range_min && range < closest) {closest = range;}
  }
  char text[64];
  std::snprintf(text, sizeof(text), "closest obstacle %.2f m", closest);
  return text;
}

template <>
inline std::string TopicMonitor<nav_msgs::msg::Odometry>::describe(
  const nav_msgs::msg::Odometry & odom) const
{
  char text[64];
  std::snprintf(
    text, sizeof(text), "position (%.2f, %.2f)",
    odom.pose.pose.position.x, odom.pose.pose.position.y);
  return text;
}
