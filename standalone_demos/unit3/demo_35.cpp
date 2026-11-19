#include <iostream>

class Battery
{
public:
  explicit Battery(double charge_percent)
  : charge_percent_{charge_percent}
  {}

  double charge_percent() const {return charge_percent_;}   // const: only reads

  void drain(double percent) {charge_percent_ -= percent;}   // not const: changes the object

private:
  double charge_percent_;
};

void print_charge(const Battery & battery)      // a const reference: read-only access
{
  std::cout << "Charge: " << battery.charge_percent() << " %\n";
}

int main()
{
  Battery main_pack{80.0};
  main_pack.drain(5.0);           // fine: main_pack is not const
  print_charge(main_pack);

  const Battery spare_pack{100.0};
  print_charge(spare_pack);       // fine: charge_percent() is const
  // spare_pack.drain(5.0);       // error: spare_pack is const, drain() is not
  return 0;
}
