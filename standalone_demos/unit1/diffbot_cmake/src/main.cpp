// Unit 1, slide 42. A small program that uses the library.
#include "diffbot_kinematics/kinematics.hpp"

int main()
{
  constexpr diffbot::RobotGeometry geometry{
    .wheel_radius_m = 0.033,
    .wheel_separation_m = 0.160,
  };

  // Drive forward at 0.2 m/s while turning left at 0.5 rad/s
  const diffbot::BodyTwist command{
    .linear_x_mps = 0.20,
    .angular_z_radps = 0.50,
  };

  diffbot::print_wheel_speeds(diffbot::to_wheel_speeds(command, geometry));
  return 0;
}
