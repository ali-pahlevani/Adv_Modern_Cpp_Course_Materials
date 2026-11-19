#include <iostream>
#include <string>

class Sensor
{
public:
  virtual ~Sensor() = default;
  virtual std::string name() const {return "generic sensor";}
};

class Lidar : public Sensor
{
public:
  std::string name() const override {return "lidar";}
};

void describe(const Sensor & sensor) {std::cout << "describe(): " << sensor.name() << '\n';}

int main()
{
  Lidar lidar;
  std::cout << "lidar.name(): " << lidar.name() << '\n';
  describe(lidar);
}
