// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.5 (Unit 2, slides 24 and 31)
// Storing the corners of a polygon in a set.
//   ros2 run exercises_unit_2 path_coordinates_set
//
// The robot drives the same pentagon twice, so every corner gets visited two times.
// Three containers record the corners. Compare their sizes at the end.

#include <chrono>
#include <cmath>
#include <compare>
#include <iostream>
#include <ostream>
#include <set>

#include <rclcpp/rclcpp.hpp>

#include "diffbot_commander/commander.hpp"

using namespace std::chrono_literals;

// A vertex as measured. The defaulted <=> (C++20) is all it needs to be a set key.
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

// The fix from slide 24: round to integer grid cells before using it as a key.
// Don't put a tolerance inside a comparator; that breaks strict weak ordering.
struct GridVertex
{
  long x_cell{};
  long y_cell{};
  auto operator<=>(const GridVertex &) const = default;
};

constexpr double kGridResolutionM = 0.10;   // 10 cm cells, so small odometry drift doesn't matter

GridVertex quantise(double x_m, double y_m)
{
  return GridVertex{
    std::lround(x_m / kGridResolutionM),
    std::lround(y_m / kGridResolutionM)};
}

std::ostream & operator<<(std::ostream & os, const GridVertex & v)
{
  return os << '[' << v.x_cell << ", " << v.y_cell << ']';
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  {
    diffbot::Commander robot;

    std::set<Vertex> raw_set;
    std::multiset<Vertex> raw_multiset;
    std::set<GridVertex> grid_set;

    constexpr int kCorners = 5;               // a pentagon
    constexpr int kLaps = 2;                  // twice round, so corners are revisited
    constexpr double kTurnRateRadps = 0.628;  // 0.628 rad/s * 2 s = 72 deg = 2*pi/5

    for (int visit = 1; visit <= kCorners * kLaps && rclcpp::ok(); ++visit) {
      robot.move_forward(2s);
      robot.turn(kTurnRateRadps, 2s);

      const Vertex v{robot.get_x_position(), robot.get_y_position()};

      const auto [raw_it, raw_new] = raw_set.insert(v);
      raw_multiset.insert(v);                                  // always inserts
      const auto [grid_it, grid_new] = grid_set.insert(quantise(v.x_m, v.y_m));

      std::cout << "visit " << visit << ": " << *raw_it
                << "   raw set: " << (raw_new ? "new" : "duplicate")
                << "   grid set: " << *grid_it << ' ' << (grid_new ? "new" : "duplicate")
                << '\n';
    }

    robot.stop_moving();

    std::cout << "\nVertices in the raw std::set (sorted by x, then y):\n";
    for (const Vertex & v : raw_set) {
      std::cout << "  " << v << '\n';
    }

    std::cout << "\nAfter " << kLaps << " laps of a " << kCorners << "-corner polygon:\n"
              << "  std::set<Vertex>      : " << raw_set.size()
              << "   <- measured doubles never repeat exactly\n"
              << "  std::multiset<Vertex> : " << raw_multiset.size()
              << "   <- so the multiset looks the same\n"
              << "  std::set<GridVertex>  : " << grid_set.size()
              << "   <- quantised: revisited corners collapse\n";
  }

  rclcpp::shutdown();
  return 0;
}
