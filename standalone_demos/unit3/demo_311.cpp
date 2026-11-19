#include <algorithm>
#include <iostream>

class Battery
{
public:
  Battery() = default;                                   // bring the default constructor back
  explicit Battery(double voltage_v) : voltage_v_{voltage_v} {}

  Battery(const Battery &) = delete;                     // a physical battery can't be duplicated
  Battery & operator=(const Battery &) = delete;

  void set_charge_percent(double percent)
  {
    charge_percent_ = std::clamp(percent, 0.0, 100.0);
  }
  void set_charge_percent(bool) = delete;                // true/false must not sneak in as 1.0/0.0

  void status() const
  {
    std::cout << voltage_v_ << " V, " << charge_percent_ << " % charged\n";
  }

private:
  double voltage_v_{12.0};
  double charge_percent_{100.0};
};

int main()
{
  Battery spare;                       // uses the defaulted constructor
  Battery main_pack{24.0};
  main_pack.set_charge_percent(80.0);

  spare.status();
  main_pack.status();

  // Battery copy = main_pack;              // error: call to deleted constructor
  // main_pack.set_charge_percent(true);    // error: call to deleted member function
  return 0;
}
