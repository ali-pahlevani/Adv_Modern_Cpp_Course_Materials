// Copyright 2026 Ali Pahlevani
// SPDX-License-Identifier: Apache-2.0
//
// The ComputerUnit given in Exercise 4.1 (Unit 4, slide 4)

#ifndef ROBOT_MANAGER_OOP__COMPUTER_UNIT_HPP_
#define ROBOT_MANAGER_OOP__COMPUTER_UNIT_HPP_

#include <fstream>
#include <sstream>
#include <string>

// CPU load and memory use of the computer it runs on, read from Linux's /proc files
class ComputerUnit
{
public:
  double memory_usage_percent() const
  {
    std::ifstream file{"/proc/meminfo"};
    double total_kb{0.0};
    double available_kb{0.0};
    for (std::string line; std::getline(file, line); ) {
      std::istringstream fields{line};
      std::string key;
      double value_kb{0.0};
      fields >> key >> value_kb;
      if (key == "MemTotal:") {total_kb = value_kb;}
      if (key == "MemAvailable:") {available_kb = value_kb;}
    }
    return total_kb > 0.0 ? 100.0 * (total_kb - available_kb) / total_kb : 0.0;
  }

  double cpu_usage_percent()      // since the previous call (the first call: since boot)
  {
    std::ifstream file{"/proc/stat"};
    std::string cpu;
    unsigned long long user{0}, nice{0}, system{0}, idle{0};
    file >> cpu >> user >> nice >> system >> idle;
    const auto busy = user + nice + system;
    const auto total = busy + idle;
    const double busy_delta = static_cast<double>(busy - last_busy_);
    const double total_delta = static_cast<double>(total - last_total_);
    last_busy_ = busy;
    last_total_ = total;
    return total_delta > 0.0 ? 100.0 * busy_delta / total_delta : 0.0;
  }

private:
  unsigned long long last_busy_{0};
  unsigned long long last_total_{0};
};

#endif  // ROBOT_MANAGER_OOP__COMPUTER_UNIT_HPP_
