#include <iostream>

class Waypoint
{
public:
  void set(double x_m, double y_m)
  {
    x_m = x_m;            // bug: assigns the parameter to itself
    this->y_m = y_m;      // correct: this->y_m is the member, y_m is the parameter
  }

  void print() const
  {
    std::cout << "Waypoint (" << x_m << ", " << y_m << ")\n";
  }

private:
  double x_m{0.0};
  double y_m{0.0};
};

int main()
{
  Waypoint goal;
  goal.set(1.5, 2.5);
  goal.print();
  return 0;
}
