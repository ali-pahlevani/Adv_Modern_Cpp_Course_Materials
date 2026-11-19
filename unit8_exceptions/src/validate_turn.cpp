// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 8.1 (Unit 8, slide 4)
//   ros2 run unit8_exceptions validate_turn

#include <iostream>
#include <stdexcept>
#include <string>

#include "unit8_exceptions/turn_validation.hpp"

int main()
{
  std::string line;
  std::cout << "Enter turn velocity: ";
  while (std::getline(std::cin, line)) {
    try {
      const double w = parse_turn_velocity(line);
      std::cout << "turning at " << w << " rad/s\n";
    } catch (const WrongTurnException & e) {         // before out_of_range: it is one
      std::cout << e.value() << " rejected, " << e.what() << '\n';
    } catch (const std::invalid_argument &) {
      std::cout << "'" << line << "' is not a number\n";
    } catch (const std::out_of_range &) {
      std::cout << "'" << line << "' does not fit in a double\n";
    }
    std::cout << "Enter turn velocity: ";
  }
}
