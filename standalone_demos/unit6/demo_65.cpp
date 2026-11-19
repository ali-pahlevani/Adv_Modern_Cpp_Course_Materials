#include <iostream>
#include <vector>

struct LaserScan {std::vector<float> ranges;};
struct Imu {double yaw_rate{0.0};};

template <typename T>                  // primary template: every type without a specialization
struct SensorInfo
{
  static constexpr const char * name = "unknown sensor";
};

template <>                            // full specialization: template<> plus the exact type
struct SensorInfo<LaserScan>
{
  static constexpr const char * name = "2D lidar";
};

template <>
struct SensorInfo<Imu>
{
  static constexpr const char * name = "IMU";
};

int main()
{
  std::cout << SensorInfo<LaserScan>::name << ", " << SensorInfo<Imu>::name << ", "
            << SensorInfo<int>::name << '\n';
}
