#include <chrono>
#include <cmath>
#include <cstdio>
#include <mutex>
#include <thread>

using namespace std::chrono_literals;

struct Pose
{
  double x{0.0};
  double y{0.0};
  double theta{0.0};
};

class PoseStore
{
public:
  void integrate(double v, double w, double dt)
  {
    std::lock_guard lock(mutex_);
    pose_.x += v * std::cos(pose_.theta) * dt;
    pose_.y += v * std::sin(pose_.theta) * dt;
    pose_.theta += w * dt;
  }

  Pose get() const
  {
    std::lock_guard lock(mutex_);
    return pose_;                                  // a copy, taken under the lock
  }

private:
  mutable std::mutex mutex_;                       // mutable: get() is const, but it locks
  Pose pose_;
};

int main()
{
  const double r = 0.033;
  const double L = 0.160;
  const double left = 4.0;                         // wheel speeds, rad/s
  const double right = 5.0;
  const double v = r * (right + left) / 2.0;       // 0.1485 m/s
  const double w = r * (right - left) / L;         // 0.20625 rad/s

  PoseStore store;

  std::jthread reader([&store](std::stop_token stop) {
      while (!stop.stop_requested()) {
        const Pose p = store.get();                // lock, copy, unlock
        std::printf("reader: x=%.3f y=%.3f theta=%.3f\n", p.x, p.y, p.theta);
        std::this_thread::sleep_for(500ms);
      }
    });

  {
    std::jthread writer([&store, v, w] {
        for (int step = 0; step < 100; ++step) {   // 50 Hz for 2 s
          store.integrate(v, w, 0.02);
          std::this_thread::sleep_for(20ms);
        }
      });
  }                                                // the writer is joined here

  reader.request_stop();
  reader.join();

  const Pose p = store.get();
  std::printf("final:  x=%.3f y=%.3f theta=%.3f\n", p.x, p.y, p.theta);
}
