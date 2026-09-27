// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0

#include "diffbot_kinematics/kinematics.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <version>   // defines the __cpp_lib_* feature-test macros

// Check the feature-test macro rather than the distro (Unit 1, slide 80).
// Humble's GCC 11 has no <format>; Jazzy's GCC 13 does.
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#include <format>
#define DIFFBOT_HAS_FORMAT 1
#else
#define DIFFBOT_HAS_FORMAT 0
#endif

namespace diffbot
{

WheelSpeeds to_wheel_speeds(
  const BodyTwist & twist, const RobotGeometry & geometry) noexcept
{
  const double half_track = 0.5 * geometry.wheel_separation_m;
  const double v_left = twist.linear_x_mps - twist.angular_z_radps * half_track;
  const double v_right = twist.linear_x_mps + twist.angular_z_radps * half_track;

  return WheelSpeeds{
    .left_radps = v_left / geometry.wheel_radius_m,
    .right_radps = v_right / geometry.wheel_radius_m,
  };
}

BodyTwist to_body_twist(
  const WheelSpeeds & wheels, const RobotGeometry & geometry) noexcept
{
  const double v_left = wheels.left_radps * geometry.wheel_radius_m;
  const double v_right = wheels.right_radps * geometry.wheel_radius_m;

  return BodyTwist{
    .linear_x_mps = 0.5 * (v_right + v_left),
    .angular_z_radps = (v_right - v_left) / geometry.wheel_separation_m,
  };
}

Pose2D integrate(const Pose2D & pose, const BodyTwist & twist, double dt_s) noexcept
{
  return Pose2D{
    .x_m = pose.x_m + twist.linear_x_mps * std::cos(pose.theta_rad) * dt_s,
    .y_m = pose.y_m + twist.linear_x_mps * std::sin(pose.theta_rad) * dt_s,
    .theta_rad = pose.theta_rad + twist.angular_z_radps * dt_s,
  };
}

void print_wheel_speeds(const WheelSpeeds & wheels)
{
#if DIFFBOT_HAS_FORMAT
  std::cout << std::format(
    "left: {:.2f} rad/s | right: {:.2f} rad/s\n", wheels.left_radps, wheels.right_radps);
#else
  // Same output as the std::format branch. A local stream keeps std::cout's
  // formatting flags untouched (std::fixed and setprecision are sticky).
  std::ostringstream line;
  line << std::fixed << std::setprecision(2)
       << "left: " << wheels.left_radps << " rad/s | right: " << wheels.right_radps << " rad/s\n";
  std::cout << line.str();
#endif
}

}  // namespace diffbot
