#include <iostream>

class PidController
{
public:
  PidController & set_kp(double kp) {kp_ = kp; return *this;}
  PidController & set_ki(double ki) {ki_ = ki; return *this;}
  PidController & set_kd(double kd) {kd_ = kd; return *this;}

  void print() const
  {
    std::cout << "kp=" << kp_ << "  ki=" << ki_ << "  kd=" << kd_ << '\n';
  }

private:
  double kp_{0.0};
  double ki_{0.0};
  double kd_{0.0};
};

int main()
{
  PidController heading_pid;
  heading_pid.set_kp(1.2).set_ki(0.1).set_kd(0.05);   // each call returns heading_pid itself
  heading_pid.print();
  return 0;
}
