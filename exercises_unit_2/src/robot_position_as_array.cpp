// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.1 (Unit 2, slide 9)
// The robot's position in a std::array.
//   ros2 run exercises_unit_2 robot_position_as_array

#include <array>
#include <iostream>

#include <rclcpp/rclcpp.hpp>

#include "diffbot_commander/commander.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  {
    diffbot::Commander robot;

    // A position always has three components, known at compile time, so std::array fits
    std::array<double, 3> position{};
    position[0] = robot.get_x_position();
    position[1] = robot.get_y_position();
    position[2] = robot.get_z_position();

    for (const double coordinate : position) {
      std::cout << coordinate << ' ';
    }
    std::cout << '\n';

    // The same thing with a structured binding
    const auto [x, y, z] = robot.get_position();
    std::cout << "x=" << x << " y=" << y << " z=" << z << '\n';
  }   // robot goes out of scope here, before shutdown, so it can still stop cleanly

  rclcpp::shutdown();
  return 0;
}
