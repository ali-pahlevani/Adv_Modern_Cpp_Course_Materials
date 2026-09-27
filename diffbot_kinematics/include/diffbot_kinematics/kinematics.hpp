// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Differential drive kinematics for diffbot. Plain C++20 with no ROS dependency, so it
// can be tested in milliseconds and reused anywhere (Unit 1, slide 30).

#ifndef DIFFBOT_KINEMATICS__KINEMATICS_HPP_
#define DIFFBOT_KINEMATICS__KINEMATICS_HPP_

namespace diffbot
{

/// Physical parameters of a differential-drive robot.
struct RobotGeometry
{
  double wheel_radius_m{0.033};
  double wheel_separation_m{0.160};
};

/// Planar body velocity: forward speed and yaw rate.
struct BodyTwist
{
  double linear_x_mps{0.0};
  double angular_z_radps{0.0};
};

/// Angular velocity of each wheel.
struct WheelSpeeds
{
  double left_radps{0.0};
  double right_radps{0.0};
};

/// A 2-D pose in the odometry frame (Unit 1, Exercise 1.1).
struct Pose2D
{
  double x_m{0.0};
  double y_m{0.0};
  double theta_rad{0.0};
};

/// Inverse kinematics: body twist -> wheel speeds.
///   v_left  = v - w * L/2      w_left  = v_left  / r
///   v_right = v + w * L/2      w_right = v_right / r
[[nodiscard]] WheelSpeeds to_wheel_speeds(
  const BodyTwist & twist, const RobotGeometry & geometry) noexcept;

/// Forward kinematics: wheel speeds -> body twist.
///   v = r * (w_right + w_left) / 2
///   w = r * (w_right - w_left) / L
[[nodiscard]] BodyTwist to_body_twist(
  const WheelSpeeds & wheels, const RobotGeometry & geometry) noexcept;

/// Euler integration of a body twist over dt_s seconds (Unit 1, Exercise 1.1).
[[nodiscard]] Pose2D integrate(
  const Pose2D & pose, const BodyTwist & twist, double dt_s) noexcept;

/// Prints both wheel speeds. Uses std::format where the standard library has it.
void print_wheel_speeds(const WheelSpeeds & wheels);

}  // namespace diffbot

#endif  // DIFFBOT_KINEMATICS__KINEMATICS_HPP_
