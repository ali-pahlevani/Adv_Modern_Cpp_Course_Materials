// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Demo 1.4 (Unit 1, slide 70)
// A node in one package using a library from another package.
//
// Terminal 1:  ros2 run diffbot_nodes wheel_speed_monitor
// Terminal 2:  ros2 topic pub -r 2 /odom nav_msgs/msg/Odometry "{twist: {twist: {linear: {x: 0.2}, angular: {z: 0.5}}}}"

#include <memory>

#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>

#include "diffbot_kinematics/kinematics.hpp"

class WheelSpeedMonitor : public rclcpp::Node
{
public:
  WheelSpeedMonitor()
  : rclcpp::Node("wheel_speed_monitor")
  {
    geometry_.wheel_radius_m =
      this->declare_parameter<double>("wheel_radius_m", 0.033);
    geometry_.wheel_separation_m =
      this->declare_parameter<double>("wheel_separation_m", 0.160);

    // SensorDataQoS (best effort) matches both reliable and best-effort publishers
    subscription_ = this->create_subscription<nav_msgs::msg::Odometry>(
      "odom", rclcpp::SensorDataQoS(),
      [this](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
        this->on_odom(*msg);
      });
  }

private:
  void on_odom(const nav_msgs::msg::Odometry & msg)
  {
    const diffbot::BodyTwist twist{
      .linear_x_mps = msg.twist.twist.linear.x,
      .angular_z_radps = msg.twist.twist.angular.z,
    };

    const auto wheels = diffbot::to_wheel_speeds(twist, geometry_);

    RCLCPP_INFO(
      this->get_logger(),
      "v=%.3f m/s  w=%.3f rad/s  ->  left=%.2f rad/s  right=%.2f rad/s",
      twist.linear_x_mps, twist.angular_z_radps,
      wheels.left_radps, wheels.right_radps);
  }

  diffbot::RobotGeometry geometry_{};
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr subscription_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<WheelSpeedMonitor>());
  rclcpp::shutdown();
  return 0;
}
