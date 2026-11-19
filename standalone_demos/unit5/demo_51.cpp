#include <iostream>

struct Pose2D {double x_m{0.0}; double y_m{0.0};};

int main()
{
  double speed_mps = 0.2;
  double * p = &speed_mps;                 // p holds the ADDRESS of speed_mps
  *p = 0.5;                                // write THROUGH the pointer
  std::cout << "p = " << p << ", *p = " << *p << ", speed_mps = " << speed_mps << '\n';

  Pose2D pose{1.0, 2.0};
  Pose2D * pose_ptr = &pose;
  pose_ptr->x_m = 3.0;                     // -> is short for (*pose_ptr).x_m
  std::cout << "pose.x_m = " << pose.x_m << '\n';

  float ranges[4]{1.0f, 1.5f, 2.0f, 2.5f};
  float * r = ranges;                      // an array decays to a pointer to its first element
  std::cout << *(r + 2) << " == " << r[2] << ", sizeof(ranges) = " << sizeof(ranges)
            << ", sizeof(r) = " << sizeof(r) << '\n';

  float * halves[2]{ranges, ranges + 2};   // an array of pointers
  std::cout << "halves[1][0] = " << halves[1][0] << '\n';
}
