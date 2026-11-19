#include <iostream>
#include <string>

int main()
{
  const double max_speed = 0.22;
  int clamped = 0;

  // [capture](parameters) -> return type {body}
  auto limit = [max_speed, &clamped](double speed) -> double {   // max_speed: a copy, clamped: a reference
    if (speed > max_speed) {++clamped; return max_speed;}
    return speed;
  };
  std::cout << limit(0.10) << ' ' << limit(0.50) << ' ' << limit(0.30) << ", clamped " << clamped << '\n';

  auto next_id = [id = 0]() mutable {return ++id;};   // init-capture: state that lives in the lambda
  next_id();
  next_id();
  std::cout << "third id: " << next_id() << '\n';

  auto show = [](const auto & value) {std::cout << value << '\n';};   // generic lambda: a template
  show(42);
  show(std::string{"odom"});
}
