// Unit 1, slide 41. The definitions, compiled once.
#include "diffbot_kinematics/kinematics.hpp"

#include <iostream>

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

void print_wheel_speeds(const WheelSpeeds & wheels)
{
  std::cout << "left: " << wheels.left_radps
            << " rad/s | right: " << wheels.right_radps << " rad/s\n";
}

}  // namespace diffbot
