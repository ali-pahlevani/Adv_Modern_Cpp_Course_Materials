// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 1.1 (Unit 1, slides 85 and 86)
// Integrates /cmd_vel at 20 Hz into an estimated pose.
//
//   ros2 run diffbot_nodes odom_integrator
//   ros2 run teleop_twist_keyboard teleop_twist_keyboard

#include <chrono>
#include <memory>

#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>

#include "diffbot_kinematics/kinematics.hpp"

using namespace std::chrono_literals;

class OdomIntegrator : public rclcpp::Node
{
public:
  OdomIntegrator()
  : rclcpp::Node("odom_integrator")
  {
    subscription_ = this->create_subscription<geometry_msgs::msg::Twist>(
      "cmd_vel", 10,
      [this](geometry_msgs::msg::Twist::ConstSharedPtr msg) {
        twist_ = diffbot::BodyTwist{
          .linear_x_mps = msg->linear.x,
          .angular_z_radps = msg->angular.z,
        };
      });

    timer_ = this->create_wall_timer(kPeriod, [this]() {step();});
  }

private:
  static constexpr auto kPeriod = 50ms;                           // 20 Hz
  static constexpr double kDtSeconds = 0.050;

  void step()
  {
    pose_ = diffbot::integrate(pose_, twist_, kDtSeconds);

    // Integrating at 20 Hz, but log only once per second
    RCLCPP_INFO_THROTTLE(
      this->get_logger(), *this->get_clock(), 1000,
      "x=%.3f m  y=%.3f m  theta=%.3f rad", pose_.x_m, pose_.y_m, pose_.theta_rad);
  }

  diffbot::Pose2D pose_{};
  diffbot::BodyTwist twist_{};
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr subscription_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<OdomIntegrator>());
  rclcpp::shutdown();
  return 0;
}
