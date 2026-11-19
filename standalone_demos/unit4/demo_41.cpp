#include <algorithm>
#include <iostream>

class Battery
{
public:
  void drain(double percent) {charge_percent_ = std::max(0.0, charge_percent_ - percent);}
  double charge_percent() const {return charge_percent_;}
private:
  double charge_percent_{100.0};
};

class Lidar
{
public:
  double closest_obstacle_m() const {return 0.42;}   // stands in for real scan data
};

class DiffBot
{
public:
  void drive_for(double seconds) {battery_.drain(1.5 * seconds);}   // hand the job to a part
  void report() const
  {
    std::cout << "battery " << battery_.charge_percent() << " %, obstacle at "
              << lidar_.closest_obstacle_m() << " m\n";
  }
private:
  Battery battery_;   // a DiffBot HAS-A Battery
  Lidar lidar_;       // and HAS-A Lidar
};

int main()
{
  DiffBot robot;
  robot.drive_for(10.0);
  robot.report();
}
