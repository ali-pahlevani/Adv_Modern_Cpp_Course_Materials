#include <iostream>
#include <string>
#include <utility>

class Robot
{
public:
  explicit Robot(std::string name) : name_{std::move(name)} {std::cout << "Robot()\n";}
  ~Robot() {std::cout << "~Robot()\n";}
  const std::string & name() const {return name_;}
private:
  std::string name_;
};

class MobileRobot : public Robot                   // a MobileRobot IS-A Robot
{
public:
  explicit MobileRobot(std::string name)
  : Robot{std::move(name)}                         // build the base first
  {std::cout << "MobileRobot()\n";}
  ~MobileRobot() {std::cout << "~MobileRobot()\n";}
};

class DiffBot : public MobileRobot                 // and a DiffBot IS-A MobileRobot
{
public:
  DiffBot() : MobileRobot{"diffbot_1"} {std::cout << "DiffBot()\n";}
  ~DiffBot() {std::cout << "~DiffBot()\n";}
};

int main()
{
  DiffBot robot;
  std::cout << "name, inherited from Robot: " << robot.name() << '\n';
}
