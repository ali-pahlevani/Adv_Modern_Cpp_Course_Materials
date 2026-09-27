// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.6 (Unit 2, slide 31)
// Storing the polygon corners in a map, keyed by corner number.
//   ros2 run exercises_unit_2 path_coordinates_map

#include <chrono>
#include <compare>
#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>

#include <rclcpp/rclcpp.hpp>

#include "diffbot_commander/commander.hpp"

using namespace std::chrono_literals;

struct Vertex
{
  double x_m{0.0};
  double y_m{0.0};
  auto operator<=>(const Vertex &) const = default;
};

std::ostream & operator<<(std::ostream & os, const Vertex & v)
{
  return os << '(' << v.x_m << ", " << v.y_m << ')';
}

// Step 4 of the exercise: change this line to
//     using VertexTable = std::unordered_map<int, Vertex>;
// then rebuild and run again. It still compiles, but the corners print in a different order.
using VertexTable = std::map<int, Vertex>;

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  {
    diffbot::Commander robot;

    VertexTable vertices;

    constexpr int kCorners = 5;
    for (int corner = 1; corner <= kCorners && rclcpp::ok(); ++corner) {
      robot.move_forward(2s);
      robot.turn(0.628, 2s);

      // emplace instead of vertices[corner] = ..., so no default Vertex is created first
      vertices.emplace(corner, Vertex{robot.get_x_position(), robot.get_y_position()});
    }

    robot.stop_moving();

    std::cout << "\nVertices formed by the robot, keyed by corner number:\n";
    for (const auto & [corner, vertex] : vertices) {
      std::cout << "  " << corner << " -> " << vertex << '\n';
    }
  }

  rclcpp::shutdown();
  return 0;
}
