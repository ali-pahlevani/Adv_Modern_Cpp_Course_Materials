// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// C++20 and C++23 examples from Unit 1, section J (slides 81 to 83).
// Everything here is header-only. std::expected only exists when the code including
// this header is compiled with -std=c++23, so it can't go into the compiled .so.

#ifndef DIFFBOT_KINEMATICS__MODERN_HPP_
#define DIFFBOT_KINEMATICS__MODERN_HPP_

#include <concepts>
#include <numeric>
#include <span>
#include <version>

#include "diffbot_kinematics/kinematics.hpp"

#if defined(__cpp_lib_expected) && __cpp_lib_expected >= 202202L
#include <expected>
#define DIFFBOT_HAS_EXPECTED 1
#else
#define DIFFBOT_HAS_EXPECTED 0
#endif

namespace diffbot
{

/// A geometry type that only accepts floating-point scalars (slide 81).
/// BasicRobotGeometry<int> won't compile, and the error message says why.
template<std::floating_point Scalar>
struct BasicRobotGeometry
{
  Scalar wheel_radius_m{};
  Scalar wheel_separation_m{};
};

using RobotGeometryf = BasicRobotGeometry<float>;   // e.g. for embedded targets

/// Mean of any contiguous block of doubles (array, vector, C array), no copying (slide 82).
[[nodiscard]] inline double mean_speed(std::span<const double> samples) noexcept
{
  if (samples.empty()) {
    return 0.0;
  }
  return std::reduce(samples.begin(), samples.end()) / static_cast<double>(samples.size());
}

#if DIFFBOT_HAS_EXPECTED
/// Returns an error value instead of throwing (slide 83).
/// Only available with -std=c++23 on Jazzy.
enum class KinematicsError
{
  zero_wheel_radius,
  zero_wheel_separation,
};

[[nodiscard]] inline std::expected<WheelSpeeds, KinematicsError>
try_to_wheel_speeds(const BodyTwist & twist, const RobotGeometry & geometry) noexcept
{
  if (geometry.wheel_radius_m <= 0.0) {
    return std::unexpected(KinematicsError::zero_wheel_radius);
  }
  if (geometry.wheel_separation_m <= 0.0) {
    return std::unexpected(KinematicsError::zero_wheel_separation);
  }
  return to_wheel_speeds(twist, geometry);
}
#endif

}  // namespace diffbot

#endif  // DIFFBOT_KINEMATICS__MODERN_HPP_
