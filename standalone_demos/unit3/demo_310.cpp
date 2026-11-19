#include <iostream>
#include <string>
#include <utility>

class MotorDriver
{
public:
  // The one constructor that does the real work
  MotorDriver(std::string joint_name, double max_speed_radps, bool inverted)
  : joint_name_{std::move(joint_name)},
    max_speed_radps_{max_speed_radps},
    inverted_{inverted}
  {
    if (max_speed_radps_ <= 0.0) {                     // validation, written once
      max_speed_radps_ = kDefaultMaxSpeedRadps;
    }
    std::cout << "[" << joint_name_ << "] ready: max " << max_speed_radps_ << " rad/s"
              << (inverted_ ? ", inverted" : "") << '\n';
  }

  // Delegating constructors: hand the work to the one above
  explicit MotorDriver(std::string joint_name)
  : MotorDriver(std::move(joint_name), kDefaultMaxSpeedRadps, false)
  {}

  MotorDriver()
  : MotorDriver("spare_joint")
  {}

private:
  static constexpr double kDefaultMaxSpeedRadps{6.67};

  std::string joint_name_;
  double max_speed_radps_;
  bool inverted_;
};

int main()
{
  MotorDriver spare;                                   // -> MotorDriver(std::string) -> main one
  MotorDriver left{"left_wheel_joint"};                // -> main one
  MotorDriver right{"right_wheel_joint", 6.0, true};   // the main one directly
  MotorDriver caster{"caster_joint", -1.0, false};     // a bad speed is fixed in one place
  return 0;
}
