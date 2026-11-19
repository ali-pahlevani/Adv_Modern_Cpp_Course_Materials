#include <iostream>
#include <string>
#include <vector>

struct RobotSpec
{
  std::string name;
  double wheel_radius_m{0.033};
  double wheel_separation_m{0.160};
  int lidar_rays{360};
  std::vector<std::string> sensors;
};

void print_spec(const RobotSpec & spec)
{
  std::cout << "Spec for '" << spec.name << "':\n";
  std::cout << "  wheel radius     : " << spec.wheel_radius_m << " m\n";
  std::cout << "  wheel separation : " << spec.wheel_separation_m << " m\n";
  std::cout << "  lidar rays       : " << spec.lidar_rays << '\n';
  std::cout << "  sensors          :";
  for (const std::string & sensor : spec.sensors) {
    std::cout << ' ' << sensor;
  }
  std::cout << '\n';
}

int main()
{
  // 1. Create an instance, then fill it in member by member
  RobotSpec diffbot_1;
  diffbot_1.name = "diffbot_1";
  diffbot_1.sensors.push_back("lidar");
  diffbot_1.sensors.push_back("imu");

  // 2. Create and fill it in one statement (C++20 designated initialisers)
  const RobotSpec diffbot_2{
    .name = "diffbot_2",
    .wheel_radius_m = 0.05,
    .sensors = {"lidar", "imu", "camera"},
  };

  print_spec(diffbot_1);
  print_spec(diffbot_2);
  return 0;
}
