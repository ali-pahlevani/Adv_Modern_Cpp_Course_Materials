// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 6.3 (Unit 6, slides 13 and 14): one describe() with concepts and if constexpr

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <ranges>
#include <string>
#include <utility>

#include <rclcpp/rclcpp.hpp>
#include <rosidl_runtime_cpp/traits.hpp>

#include "topic_monitor/moving_average.hpp"

template <typename MessageT>                          // a laser scan, whatever its exact type
concept LaserLike = requires(const MessageT & msg) {
  msg.ranges;
  msg.range_min;
  msg.range_max;
};

template <typename MessageT>                          // has a position where Odometry keeps it
concept PoseLike = requires(const MessageT & msg) {
  msg.pose.pose.position.x;
  msg.pose.pose.position.y;
};

template <typename MessageT>
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

  TopicMonitor(const TopicMonitor &) = delete;
  TopicMonitor & operator=(const TopicMonitor &) = delete;

private:
  void on_message(const MessageT & msg)
  {
    ++count_;
    RCLCPP_INFO(logger_, "[%s #%zu] %s", topic_.c_str(), count_, describe(msg).c_str());
  }

  std::string describe(const MessageT & msg)          // one function, every type
  {
    char text[80];
    if constexpr (LaserLike<MessageT>) {
      auto valid = msg.ranges | std::views::filter([&msg](float range) {
          return std::isfinite(range) && range >= msg.range_min && range <= msg.range_max;
        });
      const auto closest = std::ranges::min_element(valid);
      if (closest == valid.end()) {
        return "no valid ranges";
      }
      closest_average_.add(*closest);
      std::snprintf(
        text, sizeof(text), "closest obstacle %.2f m, smoothed %.2f m",
        *closest, closest_average_.value());
    } else if constexpr (PoseLike<MessageT>) {
      std::snprintf(
        text, sizeof(text), "position (%.2f, %.2f)",
        msg.pose.pose.position.x, msg.pose.pose.position.y);
    } else {
      return to_yaml(msg, true);
    }
    return text;
  }

  rclcpp::Logger logger_;
  std::string topic_;
  std::size_t count_{0};
  MovingAverage<float, 5> closest_average_;           // only used for laser scans
  typename rclcpp::Subscription<MessageT>::SharedPtr subscription_;
};
