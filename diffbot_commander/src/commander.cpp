// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0

#include "diffbot_commander/commander.hpp"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <thread>

#include <geometry_msgs/msg/twist.hpp>
#include <geometry_msgs/msg/twist_stamped.hpp>
#include <nav_msgs/msg/odometry.hpp>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/laser_scan.hpp>

using namespace std::chrono_literals;

namespace diffbot
{

namespace
{
// Controllers stop the robot if cmd_vel goes quiet (diff_drive_controller defaults to a
// 0.5 s timeout), so an active command is republished at 10 Hz.
constexpr auto kCommandPeriod = 100ms;
constexpr auto kSpinSlice = 50ms;

constexpr double kCircleLinearMps = 0.2;
constexpr double kCircleAngularRadps = 0.5;   // radius = v / w = 0.4 m

double yaw_from_quaternion(double x, double y, double z, double w)
{
  return std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z));
}
}  // namespace

// -------------------------------------------------------------------------------------
// All the ROS parts live here, so the public header doesn't need rclcpp.
// Member order matters: members are destroyed in reverse order, so the node (declared
// first) outlives the executor, timers and subscriptions that use it.
// -------------------------------------------------------------------------------------
struct Commander::Impl
{
  rclcpp::Node::SharedPtr node;
  rclcpp::executors::SingleThreadedExecutor executor;

  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub;
  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_pub;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr cmd_stamped_pub;
  rclcpp::TimerBase::SharedPtr cmd_timer;

  std::thread spin_thread;
  std::atomic<bool> stop_requested{false};

  // Sensor data: written by the spin thread, read from the caller's thread.
  mutable std::mutex state_mutex;
  std::condition_variable odom_cv;
  bool have_odom{false};
  std::array<double, 3> position{};
  double yaw{0.0};
  std::vector<float> ranges;

  // Current command. One mutex covers both the values and the publish call, so the
  // timer can never resend an old command after a stop.
  std::mutex command_mutex;
  double cmd_linear{0.0};
  double cmd_angular{0.0};
  bool cmd_active{false};

  double linear_speed_mps{0.2};   // set once in the constructor, then read-only

  rclcpp::Logger logger() const {return node->get_logger();}

  void command(double linear, double angular, bool active);
  void publish_locked(double linear, double angular);   // caller must hold command_mutex
  void sleep_while_ok(std::chrono::nanoseconds duration) const;
};

void Commander::Impl::publish_locked(double linear, double angular)
{
  if (cmd_stamped_pub) {
    geometry_msgs::msg::TwistStamped msg;
    msg.header.stamp = node->now();
    msg.header.frame_id = "base_link";
    msg.twist.linear.x = linear;
    msg.twist.angular.z = angular;
    cmd_stamped_pub->publish(msg);
  } else if (cmd_pub) {
    geometry_msgs::msg::Twist msg;
    msg.linear.x = linear;
    msg.angular.z = angular;
    cmd_pub->publish(msg);
  }
}

void Commander::Impl::command(double linear, double angular, bool active)
{
  const std::lock_guard lock(command_mutex);
  cmd_linear = linear;
  cmd_angular = angular;
  cmd_active = active;

  if (!rclcpp::ok()) {
    return;                       // shutting down: nothing can be published any more
  }
  try {
    publish_locked(linear, angular);
  } catch (const std::exception &) {
    if (rclcpp::ok()) {
      throw;                      // a real error
    }                             // otherwise Ctrl-C got in first; safe to ignore
  }
}

void Commander::Impl::sleep_while_ok(std::chrono::nanoseconds duration) const
{
  // Sleep in short slices so Ctrl-C interrupts a long move promptly.
  const auto deadline = std::chrono::steady_clock::now() + duration;
  while (rclcpp::ok()) {
    const auto remaining = deadline - std::chrono::steady_clock::now();
    if (remaining <= std::chrono::nanoseconds::zero()) {
      break;
    }
    std::this_thread::sleep_for(std::min<std::chrono::nanoseconds>(kSpinSlice, remaining));
  }
}

// -------------------------------------------------------------------------------------

Commander::Commander(const std::string & node_name)
{
  if (!rclcpp::ok()) {
    throw std::runtime_error(
            "diffbot::Commander: call rclcpp::init(argc, argv) before constructing it");
  }

  impl_ = std::make_unique<Impl>();
  Impl * const impl = impl_.get();   // callbacks capture this raw pointer; Impl outlives them

  impl->node = std::make_shared<rclcpp::Node>(node_name);
  rclcpp::Node & node = *impl->node;

  const auto odom_topic = node.declare_parameter<std::string>("odom_topic", "odom");
  const auto scan_topic = node.declare_parameter<std::string>("scan_topic", "scan");
  const auto cmd_vel_topic = node.declare_parameter<std::string>("cmd_vel_topic", "cmd_vel");
  const bool stamped = node.declare_parameter<bool>("use_stamped_cmd_vel", false);
  impl->linear_speed_mps = node.declare_parameter<double>("linear_speed_mps", 0.2);
  const double odom_wait_s = node.declare_parameter<double>("odom_wait_timeout_s", 5.0);

  // SensorDataQoS (best effort) is compatible with both reliable and best-effort publishers.
  impl->odom_sub = node.create_subscription<nav_msgs::msg::Odometry>(
    odom_topic, rclcpp::SensorDataQoS(),
    [impl](nav_msgs::msg::Odometry::ConstSharedPtr msg) {
      const auto & p = msg->pose.pose.position;
      const auto & q = msg->pose.pose.orientation;
      const double yaw = yaw_from_quaternion(q.x, q.y, q.z, q.w);
      {
        const std::lock_guard lock(impl->state_mutex);
        impl->position = {p.x, p.y, p.z};
        impl->yaw = yaw;
        impl->have_odom = true;
      }
      impl->odom_cv.notify_all();
    });

  impl->scan_sub = node.create_subscription<sensor_msgs::msg::LaserScan>(
    scan_topic, rclcpp::SensorDataQoS(),
    [impl](sensor_msgs::msg::LaserScan::ConstSharedPtr msg) {
      const std::lock_guard lock(impl->state_mutex);
      impl->ranges = msg->ranges;   // copy-assignment reuses capacity after the first scan
    });

  if (stamped) {
    impl->cmd_stamped_pub =
      node.create_publisher<geometry_msgs::msg::TwistStamped>(cmd_vel_topic, 10);
  } else {
    impl->cmd_pub = node.create_publisher<geometry_msgs::msg::Twist>(cmd_vel_topic, 10);
  }

  impl->cmd_timer = node.create_wall_timer(
    kCommandPeriod, [impl]() {
      const std::lock_guard lock(impl->command_mutex);
      if (impl->cmd_active) {
        impl->publish_locked(impl->cmd_linear, impl->cmd_angular);
      }
    });

  // Spin in slices rather than executor.spin(), so the destructor can always stop it.
  impl->executor.add_node(impl->node);
  impl->spin_thread = std::thread(
    [impl]() {
      while (!impl->stop_requested && rclcpp::ok()) {
        impl->executor.spin_once(kSpinSlice);
      }
    });

  // Wait for the first /odom so the first get_x_position() returns a real value.
  const auto deadline = std::chrono::steady_clock::now() +
    std::chrono::duration_cast<std::chrono::nanoseconds>(
    std::chrono::duration<double>(odom_wait_s));
  bool got_odom = false;
  {
    std::unique_lock lock(impl->state_mutex);
    while (!impl->have_odom && rclcpp::ok() && std::chrono::steady_clock::now() < deadline) {
      impl->odom_cv.wait_for(lock, 100ms);
    }
    got_odom = impl->have_odom;
  }

  if (got_odom) {
    RCLCPP_INFO(impl->logger(), "Commander ready");
  } else {
    RCLCPP_WARN(
      impl->logger(),
      "Commander ready, but no odometry on '%s' after %.1f s. Position and heading "
      "will read 0 until it arrives. Is the simulator running?",
      impl->odom_sub->get_topic_name(), odom_wait_s);
  }
}

Commander::~Commander()
{
  if (!impl_) {
    return;
  }
  try {
    impl_->command(0.0, 0.0, false);   // never leave the robot driving
  } catch (...) {
    // destructors must not throw
  }
  impl_->stop_requested = true;
  if (impl_->spin_thread.joinable()) {
    impl_->spin_thread.join();
  }
}

// ---- pose -----------------------------------------------------------------------------

std::array<double, 3> Commander::get_position() const
{
  const std::lock_guard lock(impl_->state_mutex);
  return impl_->position;
}

double Commander::get_x_position() const {return get_position()[0];}
double Commander::get_y_position() const {return get_position()[1];}
double Commander::get_z_position() const {return get_position()[2];}

double Commander::get_heading() const
{
  const std::lock_guard lock(impl_->state_mutex);
  return impl_->yaw;
}

// ---- sensors --------------------------------------------------------------------------

std::vector<float> Commander::get_laser_ranges() const
{
  const std::lock_guard lock(impl_->state_mutex);
  return impl_->ranges;
}

// ---- motion ---------------------------------------------------------------------------

void Commander::drive(double linear_x_mps, double angular_z_radps)
{
  impl_->command(linear_x_mps, angular_z_radps, true);
}

void Commander::move_forward(std::chrono::seconds duration)
{
  RCLCPP_INFO(
    impl_->logger(), "Moving forward for %lld s", static_cast<long long>(duration.count()));
  impl_->command(impl_->linear_speed_mps, 0.0, true);
  impl_->sleep_while_ok(duration);
  impl_->command(0.0, 0.0, false);
}

void Commander::move_backward(std::chrono::seconds duration)
{
  RCLCPP_INFO(
    impl_->logger(), "Moving backward for %lld s", static_cast<long long>(duration.count()));
  impl_->command(-impl_->linear_speed_mps, 0.0, true);
  impl_->sleep_while_ok(duration);
  impl_->command(0.0, 0.0, false);
}

void Commander::turn(double angular_z_radps, std::chrono::seconds duration)
{
  RCLCPP_INFO(
    impl_->logger(), "Turning at %.3f rad/s for %lld s",
    angular_z_radps, static_cast<long long>(duration.count()));
  impl_->command(0.0, angular_z_radps, true);
  impl_->sleep_while_ok(duration);
  impl_->command(0.0, 0.0, false);
}

void Commander::move_in_circles()
{
  RCLCPP_INFO(impl_->logger(), "Moving in circles");
  impl_->command(kCircleLinearMps, kCircleAngularRadps, true);
}

void Commander::stop_moving()
{
  RCLCPP_INFO(impl_->logger(), "Stopping the robot");
  impl_->command(0.0, 0.0, false);
}

bool Commander::ok() const
{
  return rclcpp::ok() && !impl_->stop_requested;
}

}  // namespace diffbot
