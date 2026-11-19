#include <functional>
#include <iostream>
#include <vector>

std::vector<std::function<void()>> callbacks;     // stands in for a node's timers

class Monitor
{
public:
  void start()
  {
    double max_speed = 0.22;
    callbacks.push_back([&max_speed]() {std::cout << "limit: " << max_speed << '\n';});  // BUG
    callbacks.push_back([=]() {++count_;});      // [=] silently captures this, not count_
  }
  int count_{0};
};

int main()
{
  Monitor monitor;
  monitor.start();                                // start() has returned: max_speed is gone
  callbacks[1]();
  callbacks[1]();
  std::cout << "count_ = " << monitor.count_ << '\n';
  callbacks[0]();                                 // the "timer" fires later
}
