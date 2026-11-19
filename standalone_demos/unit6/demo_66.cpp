#include <algorithm>
#include <iostream>
#include <vector>

struct LaserScan {std::vector<float> ranges;};
struct Odometry {struct {struct {struct {double x, y;} position;} pose;} pose;};
struct Battery {double voltage;};

template <typename Msg>                                // a custom concept: "has a ranges array"
concept LaserLike = requires(const Msg & msg) {
  msg.ranges.size();
};

template <typename Msg>                                // "has a position, where Odometry keeps it"
concept PoseLike = requires(const Msg & msg) {
  msg.pose.pose.position.x;
};

template <typename Msg>
void describe(const Msg & msg)
{
  if constexpr (LaserLike<Msg>) {                      // decided at compile time:
    std::cout << "closest obstacle " << std::ranges::min(msg.ranges) << " m\n";
  } else if constexpr (PoseLike<Msg>) {                // the other branches are not compiled
    std::cout << "position x " << msg.pose.pose.position.x << " m\n";
  } else {
    std::cout << "no description\n";
  }
}

int main()
{
  describe(LaserScan{{1.5f, 0.8f, 2.0f}});
  describe(Odometry{{{{0.5, 0.2}}}});
  describe(Battery{12.4});
  static_assert(LaserLike<LaserScan> && !LaserLike<Battery>);
}
