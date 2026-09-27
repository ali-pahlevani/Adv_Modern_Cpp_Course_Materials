// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Tests for Unit 1, slide 68 and Exercise 1.1. No ROS involved, so they run in milliseconds.

#include <gtest/gtest.h>

#include <array>
#include <span>
#include <vector>

#include "diffbot_kinematics/kinematics.hpp"
#include "diffbot_kinematics/modern.hpp"

namespace
{
constexpr diffbot::RobotGeometry kGeometry{};   // r = 0.033 m, L = 0.160 m
}  // namespace

TEST(Kinematics, StraightLineMeansEqualWheels)
{
  const auto w = diffbot::to_wheel_speeds({.linear_x_mps = 0.5}, kGeometry);
  EXPECT_DOUBLE_EQ(w.left_radps, w.right_radps);
  EXPECT_GT(w.left_radps, 0.0);
}

TEST(Kinematics, SpinInPlaceMeansOppositeWheels)
{
  const auto w = diffbot::to_wheel_speeds({.angular_z_radps = 1.0}, kGeometry);
  EXPECT_DOUBLE_EQ(w.left_radps, -w.right_radps);
}

TEST(Kinematics, TurningLeftSpinsRightWheelFaster)
{
  const auto w = diffbot::to_wheel_speeds(
    {.linear_x_mps = 0.20, .angular_z_radps = 0.50}, kGeometry);
  EXPECT_GT(w.right_radps, w.left_radps);
  EXPECT_NEAR(w.left_radps, 4.8485, 1e-3);    // matches Unit 1, slide 42
  EXPECT_NEAR(w.right_radps, 7.2727, 1e-3);
}

TEST(Kinematics, RoundTripIsIdentity)
{
  const diffbot::BodyTwist in{.linear_x_mps = 0.2, .angular_z_radps = 0.5};
  const auto out = diffbot::to_body_twist(diffbot::to_wheel_speeds(in, kGeometry), kGeometry);
  EXPECT_NEAR(out.linear_x_mps, in.linear_x_mps, 1e-9);
  EXPECT_NEAR(out.angular_z_radps, in.angular_z_radps, 1e-9);
}

// Exercise 1.1: driving straight at 0.5 m/s for 1 s from the origin should end near x = 0.5, y = 0
TEST(Kinematics, IntegrateStraightLine)
{
  diffbot::Pose2D pose{};
  const diffbot::BodyTwist twist{.linear_x_mps = 0.5};
  for (int step = 0; step < 20; ++step) {
    pose = diffbot::integrate(pose, twist, 0.05);
  }
  EXPECT_NEAR(pose.x_m, 0.5, 1e-9);
  EXPECT_NEAR(pose.y_m, 0.0, 1e-9);
  EXPECT_NEAR(pose.theta_rad, 0.0, 1e-9);
}

TEST(Modern, MeanSpeedAcceptsAnyContiguousContainer)
{
  const std::array<double, 3> a{1.0, 2.0, 3.0};
  const std::vector<double> v{1.0, 2.0, 3.0};
  EXPECT_DOUBLE_EQ(diffbot::mean_speed(a), 2.0);
  EXPECT_DOUBLE_EQ(diffbot::mean_speed(v), 2.0);
  EXPECT_DOUBLE_EQ(diffbot::mean_speed(std::span<const double>{}), 0.0);
}

#if DIFFBOT_HAS_EXPECTED
TEST(Modern, ExpectedRejectsZeroRadius)
{
  const auto result = diffbot::try_to_wheel_speeds({}, {.wheel_radius_m = 0.0});
  ASSERT_FALSE(result.has_value());
  EXPECT_EQ(result.error(), diffbot::KinematicsError::zero_wheel_radius);
}
#endif
