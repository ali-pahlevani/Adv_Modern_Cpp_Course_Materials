#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ISensor                                    // an interface
{
public:
  virtual ~ISensor() = default;
  virtual std::string name() const = 0;          // pure virtual: every sensor must provide it
  virtual bool healthy() const = 0;
};

class Lidar final : public ISensor
{
public:
  std::string name() const override {return "lidar";}
  bool healthy() const override {return true;}
};

class Camera final : public ISensor
{
public:
  std::string name() const override {return "camera";}
  bool healthy() const override {return false;}   // say it's unplugged
};

int main()
{
  std::vector<std::unique_ptr<ISensor>> sensors;
  sensors.push_back(std::make_unique<Lidar>());
  sensors.push_back(std::make_unique<Camera>());

  for (const auto & sensor : sensors) {          // one loop, any kind of sensor
    std::cout << sensor->name() << ": " << (sensor->healthy() ? "ok" : "FAULT") << '\n';
  }
}
