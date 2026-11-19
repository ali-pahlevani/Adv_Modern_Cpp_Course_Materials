#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>

int unsafe_count = 0;                              // shared, no protection
int locked_count = 0;                              // shared, protected by count_mutex
std::mutex count_mutex;
std::atomic<int> atomic_count{0};                  // shared, atomic

void count_scans()
{
  for (int i = 0; i < 1'000'000; ++i) {
    ++unsafe_count;                                // data race: undefined behaviour
  }
  for (int i = 0; i < 1'000'000; ++i) {
    std::lock_guard lock(count_mutex);             // locks here...
    ++locked_count;
  }                                                // ...and unlocks at the end of each pass
  for (int i = 0; i < 1'000'000; ++i) {
    ++atomic_count;                                // one indivisible operation
  }
}

int main()
{
  std::thread a(count_scans);
  std::thread b(count_scans);
  a.join();
  b.join();

  std::cout << "expected: 2000000\n";
  std::cout << "unsafe:   " << unsafe_count << '\n';
  std::cout << "locked:   " << locked_count << '\n';
  std::cout << "atomic:   " << atomic_count << '\n';
}
