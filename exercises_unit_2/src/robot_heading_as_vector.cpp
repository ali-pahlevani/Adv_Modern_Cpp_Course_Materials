// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.2 (Unit 2, slide 16)
// Recording the robot's heading in a std::vector.
//   ros2 run exercises_unit_2 robot_heading_as_vector

#include <algorithm>
#include <chrono>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "diffbot_commander/commander.hpp"

using namespace std::chrono_literals;

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  {
    diffbot::Commander robot;
    robot.move_in_circles();

    int total_values = 0;
    std::cout << "How many heading values should I capture? ";
    std::cin >> total_values;
    total_values = std::max(total_values, 0);   // a negative count would wrap when cast

    // ===== SOLUTION =====
    std::vector<float> headings;
    headings.reserve(static_cast<std::size_t>(total_values));   // one allocation, before the loop

    while (std::ssize(headings) < total_values && rclcpp::ok()) {
      headings.push_back(static_cast<float>(robot.get_heading()));
      std::cout << "Collecting heading value: " << headings.back() << '\n';
      std::this_thread::sleep_for(1s);
    }

    std::cout << "\nDisplaying saved data\n";
    for (std::size_t i = 0; i < headings.size(); ++i) {
      std::cout << "Heading value " << i << ": " << headings[i] << '\n';
    }
    // ===== END SOLUTION =====

    robot.stop_moving();
  }

  rclcpp::shutdown();
  return 0;
}
