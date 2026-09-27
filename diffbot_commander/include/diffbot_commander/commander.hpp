// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// A simple blocking API for driving diffbot from a plain main().
// All the Unit 2 exercises use it (slide 5), so students can work on containers
// without having to deal with executors yet.
//
// The class owns a ROS 2 node and spins it on a background thread, so odometry and
// laser messages keep arriving while main() sleeps or waits for std::cin.
//
// Usage:
//   rclcpp::init(argc, argv);          // first
//   diffbot::Commander robot;          // waits (up to 5 s) for the first /odom
//   robot.move_forward(2s);
//   double yaw = robot.get_heading();
//   robot.stop_moving();
//   rclcpp::shutdown();                // last
//
// Parameters (set with --ros-args -p name:=value):
//   odom_topic           (string, "odom")
//   scan_topic           (string, "scan")
//   cmd_vel_topic        (string, "cmd_vel")
//   use_stamped_cmd_vel  (bool,   false)  publish TwistStamped instead of Twist
//   linear_speed_mps     (double, 0.2)    speed used by move_forward / move_backward
//   odom_wait_timeout_s  (double, 5.0)    how long the constructor waits for /odom

#ifndef DIFFBOT_COMMANDER__COMMANDER_HPP_
#define DIFFBOT_COMMANDER__COMMANDER_HPP_

#include <array>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

namespace diffbot
{

class Commander
{
public:
  explicit Commander(const std::string & node_name = "diffbot_commander");
  ~Commander();

  // Owns a thread and a node: neither copyable nor movable.
  Commander(const Commander &) = delete;
  Commander & operator=(const Commander &) = delete;
  Commander(Commander &&) = delete;
  Commander & operator=(Commander &&) = delete;

  // ---- pose (from the latest /odom) -------------------------------------------------
  [[nodiscard]] double get_x_position() const;
  [[nodiscard]] double get_y_position() const;
  [[nodiscard]] double get_z_position() const;
  [[nodiscard]] std::array<double, 3> get_position() const;
  [[nodiscard]] double get_heading() const;          // yaw in radians, [-pi, pi]

  // ---- sensors ----------------------------------------------------------------------
  /// Latest LaserScan.ranges. Empty until the first scan arrives; the constructor only
  /// waits for odometry, and the laser often starts later. Check .empty() first.
  [[nodiscard]] std::vector<float> get_laser_ranges() const;

  // ---- motion -----------------------------------------------------------------------
  /// Non-blocking: keep driving at this twist until told otherwise.
  void drive(double linear_x_mps, double angular_z_radps);

  /// Blocking: drive for `duration`, then stop.
  void move_forward(std::chrono::seconds duration);
  void move_backward(std::chrono::seconds duration);
  void turn(double angular_z_radps, std::chrono::seconds duration);

  /// Non-blocking: drive in a circle until stop_moving() is called.
  void move_in_circles();

  /// Stop the robot and stop publishing commands.
  void stop_moving();

  /// False once ROS 2 is shutting down (e.g. after Ctrl-C).
  [[nodiscard]] bool ok() const;

private:
  struct Impl;                     // pimpl: keeps rclcpp out of this header
  std::unique_ptr<Impl> impl_;
};

}  // namespace diffbot

#endif  // DIFFBOT_COMMANDER__COMMANDER_HPP_
