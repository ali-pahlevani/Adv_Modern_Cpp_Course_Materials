#include <iostream>

struct Pose2D {double x_m{0.0}; double y_m{0.0};};

void move_forward(Pose2D & pose, double distance_m) {pose.x_m += distance_m;}      // reference
void move_forward(Pose2D * pose, double distance_m) {if (pose) {pose->x_m += distance_m;}}   // pointer
void print(const Pose2D & pose) {std::cout << "(" << pose.x_m << ", " << pose.y_m << ")\n";}  // const reference

int main()
{
  Pose2D pose;
  Pose2D & alias = pose;                    // another name for pose: must be set right here
  alias.y_m = 1.0;

  move_forward(pose, 0.5);                  // no & at the call: it still changes pose
  move_forward(&pose, 0.5);                 // the pointer version needs &
  print(pose);

  const Pose2D other{9.0, 9.0};
  alias = other;                            // NOT re-pointing alias: copies other INTO pose
  print(pose);
}
