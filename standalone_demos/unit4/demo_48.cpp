#include <bit>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>

struct ISensor {virtual ~ISensor() = default;};
struct Lidar final : ISensor {int rays() const {return 360;}};   // only a Lidar has rays()
struct Imu final : ISensor {};

int main()
{
  // static_cast: a conversion you mean, checked at compile time
  const int ticks = 1000;
  const int ticks_per_rev = 3;
  std::cout << ticks / ticks_per_rev << " vs " << static_cast<double>(ticks) / ticks_per_rev << '\n';

  // dynamic_cast: "is this ISensor really a Lidar?", checked at run time
  std::vector<std::unique_ptr<ISensor>> sensors;
  sensors.push_back(std::make_unique<Lidar>());
  sensors.push_back(std::make_unique<Imu>());
  for (const auto & sensor : sensors) {
    if (const auto * lidar = dynamic_cast<const Lidar *>(sensor.get())) {
      std::cout << "a Lidar with " << lidar->rays() << " rays\n";
    } else {
      std::cout << "not a Lidar: dynamic_cast gave nullptr\n";
    }
  }

  // std::bit_cast (C++20): look at the bytes of a value, safely
  const float range_m = 1.25f;
  std::cout << std::hex << "bits of 1.25f: 0x" << std::bit_cast<std::uint32_t>(range_m) << '\n';
}
