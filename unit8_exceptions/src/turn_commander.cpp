// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 8.2 (Unit 8, slide 6)
//   ros2 run unit8_exceptions turn_commander
//   ros2 topic pub --once /turn_cmd std_msgs/msg/String "{data: '-0.5'}"

#include <memory>
#include <stdexcept>
#include <string>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "unit8_exceptions/turn_validation.hpp"

class TurnCommander : public rclcpp::Node
{
public:
  TurnCommander()
  : Node("turn_commander")
  {
    cmd_pub_ = create_publisher<geometry_msgs::msg::Twist>("cmd_vel", 10);
    text_sub_ = create_subscription<std_msgs::msg::String>(
      "turn_cmd", 10, [this](std_msgs::msg::String::ConstSharedPtr msg) {on_text(msg->data);});
  }

private:
  void on_text(const std::string & text)
  {
    try {
      geometry_msgs::msg::Twist cmd;
      cmd.angular.z = parse_turn_velocity(text);
      cmd_pub_->publish(cmd);
      RCLCPP_INFO(get_logger(), "turning at %.2f rad/s", cmd.angular.z);
    } catch (const WrongTurnException & e) {         // a bad command: warn and keep running
      RCLCPP_WARN(get_logger(), "%.2f rejected, %s", e.value(), e.what());
    } catch (const std::invalid_argument &) {
      RCLCPP_WARN(get_logger(), "'%s' is not a number", text.c_str());
    } catch (const std::out_of_range &) {
      RCLCPP_WARN(get_logger(), "'%s' does not fit in a double", text.c_str());
    }
  }

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub_;
  rclcpp::Subscription<std_msgs::msg::String>::SharedPtr text_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  int exit_code = 0;
  try {
    rclcpp::spin(std::make_shared<TurnCommander>());
  } catch (const std::exception & e) {               // anything the callback did not handle
    RCLCPP_FATAL(rclcpp::get_logger("turn_commander"), "%s", e.what());
    exit_code = 1;
  }
  rclcpp::shutdown();
  return exit_code;
}
