// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// Exercise 2.4 (Unit 2, slide 19)
// Moving through a std::list with iterators.
//   ros2 run exercises_unit_2 laser_fields_as_list
//
// No robot needed, this one is just iterator practice.

#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <string>

int main()
{
  // ---- std::list: bidirectional iterators --------------------------------------------
  const std::list<std::string> laser{"range_max", "range_min", "angle_max", "angle_min"};

  // A list has no operator[], so we walk with an iterator
  auto it = laser.begin();
  std::advance(it, 2);                          // begin() is already element 0
  std::cout << "list, 3rd from front: " << *it << '\n';                 // angle_max

  // end() is one past the last element, so step back 3
  std::cout << "list, 3rd from end  : " << *std::prev(laser.end(), 3) << '\n';   // range_min

  // ---- std::forward_list: forward iterators only ---------------------------------------
  const std::forward_list<std::string> flaser{
    "range_max", "range_min", "angle_max", "angle_min"};

  auto fit = flaser.begin();
  std::advance(fit, 2);                         // moving forward is fine
  std::cout << "forward_list, 3rd from front: " << *fit << '\n';        // angle_max

  // Don't do this. It compiles, but it's undefined behaviour (usually a segfault)
  // because a forward iterator can't go backwards:
  //   auto bad = std::next(flaser.end(), -3);
  //
  // Instead: forward_list has no size(), so count the elements first, then walk again.
  const auto count = std::distance(flaser.begin(), flaser.end());       // O(n)
  const auto third_from_end = std::next(flaser.begin(), count - 3);
  std::cout << "forward_list, 3rd from end  : " << *third_from_end << '\n';   // range_min

  return 0;
}
