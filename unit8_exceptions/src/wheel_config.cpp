// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Demo 8.3 (Unit 8, slide 5): exceptions in a ROS 2 node
//   ros2 run unit8_exceptions wheel_config --ros-args -p wheel_radius:=small

#include <memory>
#include <stdexcept>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

class WheelConfig : public rclcpp::Node
{
public:
  WheelConfig()
  : Node("wheel_config")
  {
    radius_ = declare_parameter("wheel_radius", 0.033);  // a string here throws
    if (radius_ <= 0.0) {
      throw std::invalid_argument("wheel_radius must be above 0");
    }
    speed_sub_ = create_subscription<std_msgs::msg::Float64>(
      "wheel_speed", 10, [this](std_msgs::msg::Float64::ConstSharedPtr msg) {on_speed(msg->data);});
    RCLCPP_INFO(get_logger(), "wheel_radius: %.3f m", radius_);
  }

private:
  void on_speed(double speed)
  {
    if (speed > 10.0) {
      throw std::out_of_range("wheel speed above 10 rad/s");   // leaves the callback...
    }
    RCLCPP_INFO(get_logger(), "linear speed: %.3f m/s", speed * radius_);
  }

  double radius_{};
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr speed_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int exit_code = 0;
  try {
    rclcpp::spin(std::make_shared<WheelConfig>());     // ...and comes out of spin() here
  } catch (const std::exception & e) {
    RCLCPP_FATAL(rclcpp::get_logger("wheel_config"), "%s", e.what());
    exit_code = 1;
  }
  rclcpp::shutdown();
  return exit_code;
}
