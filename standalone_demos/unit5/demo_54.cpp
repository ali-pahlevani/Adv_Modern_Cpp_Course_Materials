#include <iostream>
#include <memory>
#include <vector>

struct LaserScan {std::vector<float> ranges;};

const LaserScan * latest_scan = nullptr;    // a raw pointer, kept after the callback

void scan_callback(std::shared_ptr<const LaserScan> msg)
{
  latest_scan = msg.get();                  // borrows the scan, doesn't own it
}                                           // msg was the last owner: the scan is freed HERE

int main()
{
  scan_callback(std::make_shared<const LaserScan>(LaserScan{{1.25f, 2.5f}}));
  std::cout << "front: " << latest_scan->ranges[0] << '\n';   // reads freed memory
}
