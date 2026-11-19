#include <iostream>
#include <memory>
#include <vector>

class Sensor
{
public:
  ~Sensor() {std::cout << "~Sensor()\n";}                 // NOT virtual
  virtual const char * name() const {return "sensor";}
};

class Lidar : public Sensor
{
public:
  ~Lidar() {std::cout << "~Lidar(): scan buffer freed\n";}
  const char * name() const override {return "lidar";}
private:
  std::vector<float> ranges_ = std::vector<float>(360);
};

int main()
{
  std::unique_ptr<Sensor> sensor = std::make_unique<Lidar>();
  std::cout << sensor->name() << '\n';
}                                                          // deletes a Lidar through a Sensor *
