#include <iostream>
#include <string>
#include <utility>

class MotorDriver
{
public:
  static constexpr double kMaxSpeedRadps{6.67};   // a constant shared by every motor

  explicit MotorDriver(std::string joint_name)
  : joint_name_{std::move(joint_name)}
  {
    ++alive_count_;
    std::cout << joint_name_ << " enabled  (" << alive_count_ << " alive)\n";
  }

  ~MotorDriver()
  {
    --alive_count_;
    std::cout << joint_name_ << " disabled (" << alive_count_ << " alive)\n";
  }

  static int alive_count() {return alive_count_;}   // static member function

private:
  inline static int alive_count_{0};   // C++17: declared AND defined here
  std::string joint_name_;
};

int main()
{
  // No object needed for static members:
  std::cout << "Max speed: " << MotorDriver::kMaxSpeedRadps << " rad/s\n";
  std::cout << "Alive at start: " << MotorDriver::alive_count() << '\n';

  MotorDriver left{"left_wheel_joint"};
  {
    MotorDriver right{"right_wheel_joint"};
    std::cout << "Alive inside the block: " << MotorDriver::alive_count() << '\n';
  }
  std::cout << "Alive after the block: " << MotorDriver::alive_count() << '\n';
  return 0;
}
