#include <iostream>
#include <string>
#include <utility>

class MotorDriver
{
public:
  // Default constructor
  MotorDriver()
  : joint_name_{"spare_joint"}, max_speed_radps_{6.0}
  {
    std::cout << "[" << joint_name_ << "] default constructor: motor enabled\n";
  }

  // Parameterised constructor
  MotorDriver(std::string joint_name, double max_speed_radps)
  : joint_name_{std::move(joint_name)}, max_speed_radps_{max_speed_radps}
  {
    std::cout << "[" << joint_name_ << "] parameterised constructor: motor enabled\n";
  }

  // Destructor
  ~MotorDriver()
  {
    std::cout << "[" << joint_name_ << "] destructor: motor disabled\n";
  }

  void status() const
  {
    std::cout << "[" << joint_name_ << "] max speed " << max_speed_radps_ << " rad/s\n";
  }

private:
  std::string joint_name_;
  double max_speed_radps_;
};

int main()
{
  std::cout << "main starts\n";
  MotorDriver spare;                                  // default constructor
  MotorDriver left{"left_wheel_joint", 6.67};         // parameterised constructor

  {
    MotorDriver right{"right_wheel_joint", 6.67};
    right.status();
    std::cout << "leaving the inner scope\n";
  }                                                   // right is destroyed here

  spare.status();
  left.status();
  std::cout << "main ends\n";
  return 0;
}                                                     // left, then spare, are destroyed here
