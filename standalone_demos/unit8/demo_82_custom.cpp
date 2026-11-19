#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

class DiffbotError : public std::runtime_error       // base class for all diffbot errors
{
public:
  using std::runtime_error::runtime_error;           // reuse its constructor: (message)
};

class WheelSpeedError : public DiffbotError          // carries extra data
{
public:
  WheelSpeedError(const std::string & wheel, double speed)
  : DiffbotError(wheel + " wheel above 10 rad/s"), speed_{speed} {}

  double speed() const noexcept {return speed_;}

private:
  double speed_;
};

class EmergencyStop : public std::exception          // overrides what() itself
{
public:
  const char * what() const noexcept override {return "emergency stop pressed";}
};

void check_wheels(double left, double right)
{
  if (left > 10.0) {throw WheelSpeedError("left", left);}
  if (right > 10.0) {throw WheelSpeedError("right", right);}
}

int main()
{
  for (const double right : {4.0, 12.5}) {
    try {
      check_wheels(4.0, right);
      std::cout << "wheels ok\n";
      throw EmergencyStop{};
    } catch (const WheelSpeedError & e) {            // derived class before its base
      std::cout << "WheelSpeedError: " << e.what() << ", speed " << e.speed() << '\n';
    } catch (const DiffbotError & e) {
      std::cout << "DiffbotError: " << e.what() << '\n';
    } catch (const std::exception & e) {             // EmergencyStop lands here
      std::cout << "std::exception: " << e.what() << '\n';
    }
  }
}
