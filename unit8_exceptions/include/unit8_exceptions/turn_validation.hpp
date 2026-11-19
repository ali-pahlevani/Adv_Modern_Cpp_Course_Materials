// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 8.1 (Unit 8, slide 4): WrongTurnException and parse_turn_velocity()

#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

class WrongTurnException : public std::out_of_range
{
public:
  explicit WrongTurnException(double value)
  : std::out_of_range("allowed turn velocity: -1.0 to 0.0 rad/s"), value_{value} {}

  double value() const noexcept {return value_;}

private:
  double value_;
};

inline double parse_turn_velocity(const std::string & text)
{
  std::size_t used = 0;
  const double value = std::stod(text, &used);       // throws invalid_argument or out_of_range
  if (used != text.size()) {
    throw std::invalid_argument("extra characters after the number");
  }
  if (value < -1.0 || value > 0.0) {
    throw WrongTurnException(value);
  }
  return value;
}
