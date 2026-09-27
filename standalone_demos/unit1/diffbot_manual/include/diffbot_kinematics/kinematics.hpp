// Unit 1, slide 39. The library's public interface.
#ifndef DIFFBOT_KINEMATICS__KINEMATICS_HPP_
#define DIFFBOT_KINEMATICS__KINEMATICS_HPP_

namespace diffbot
{

struct RobotGeometry
{
  double wheel_radius_m{0.033};
  double wheel_separation_m{0.160};
};

struct BodyTwist
{
  double linear_x_mps{0.0};
  double angular_z_radps{0.0};
};

struct WheelSpeeds
{
  double left_radps{0.0};
  double right_radps{0.0};
};

[[nodiscard]] WheelSpeeds to_wheel_speeds(
  const BodyTwist & twist, const RobotGeometry & geometry) noexcept;

[[nodiscard]] BodyTwist to_body_twist(
  const WheelSpeeds & wheels, const RobotGeometry & geometry) noexcept;

void print_wheel_speeds(const WheelSpeeds & wheels);

}  // namespace diffbot

#endif  // DIFFBOT_KINEMATICS__KINEMATICS_HPP_
