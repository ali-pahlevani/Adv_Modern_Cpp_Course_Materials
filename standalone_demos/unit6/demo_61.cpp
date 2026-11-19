#include <iostream>
#include <string>

template <typename T>                                  // T: a placeholder for any type
T clamp_value(T value, T low, T high)
{
  return value < low ? low : (value > high ? high : value);
}

void log_value(const std::string & name, const auto & value)   // C++20: auto makes it a template
{
  std::cout << name << " = " << value << '\n';
}

int main()
{
  log_value("speed", clamp_value(0.35, -0.22, 0.22));     // T deduced as double
  log_value("rays", clamp_value(400, 0, 360));            // T deduced as int
  log_value("frame", std::string{"base_link"});
  log_value("explicit", clamp_value<double>(1, 0, 0.5));  // T given: 1 and 0 convert to double
}
