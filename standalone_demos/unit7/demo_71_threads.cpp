#include <chrono>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

void add_suffix(std::string & frame)               // takes a reference
{
  frame += "_filtered";
}

int count_close(std::vector<float> ranges, float limit)   // returns a value
{
  std::this_thread::sleep_for(50ms);               // pretend this is slow
  int close = 0;
  for (const float r : ranges) {
    if (r < limit) {++close;}
  }
  return close;
}

int main()
{
  // 1. std::thread: start it, then join it
  std::string frame{"laser"};
  std::thread worker(add_suffix, std::ref(frame)); // std::ref: pass a real reference
  worker.join();                                   // wait here until it finishes
  std::cout << "frame: " << frame << '\n';

  // 2. std::jthread (C++20): joins itself, and can be asked to stop
  {
    std::jthread heartbeat([](std::stop_token stop) {
        while (!stop.stop_requested()) {
          std::this_thread::sleep_for(10ms);
        }
        std::cout << "heartbeat stopped\n";
      });
    std::this_thread::sleep_for(50ms);
  }                                                // here: request_stop(), then join()

  // 3. std::async: run a function on another thread and get its result later
  const std::vector<float> ranges{0.3f, 1.2f, 0.4f, 2.5f};
  std::future<int> close = std::async(std::launch::async, count_close, ranges, 0.5f);
  std::cout << "main keeps working\n";
  std::cout << "close rays: " << close.get() << '\n';   // waits for the result
}
