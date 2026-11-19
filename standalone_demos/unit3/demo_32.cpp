#include <iostream>

class Battery
{
  double voltage_v;
  double charge_percent;

  void status()
  {
    std::cout << "Battery at " << voltage_v << " V, " << charge_percent << " % charged\n";
  }
};

int main()
{
  Battery battery;                  // create an object (an instance) of class Battery
  battery.voltage_v = 12.4;         // assign its member variables
  battery.charge_percent = 87.0;
  battery.status();                 // call a member function
  return 0;
}
