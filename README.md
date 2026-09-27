# Advanced Modern C++ for Robotics & ROS 2

Code for the course. All the examples use the same robot, diffbot: a small two-wheeled
differential drive robot with a wheel radius of 0.033 m and a wheel separation of 0.160 m.

Written for ROS 2 Humble (Ubuntu 22.04) and Jazzy (Ubuntu 24.04). The code is C++20, with a
few optional C++23 parts that switch on when the compiler supports them.

So far the repo covers Unit 1 (Build Tools) and Unit 2 (The STL Library).

## What's in the repo

| Folder | Unit | Contents |
|---|---|---|
| `diffbot_kinematics` | 1 | Kinematics library in plain C++20 (no ROS), with GTest tests |
| `diffbot_nodes` | 1 | ROS 2 nodes that use the library, including the Exercise 1.1 solution |
| `diffbot_commander` | 2 | Helper class for driving the robot from a normal `main()` |
| `exercises_unit_2` | 2 | Solutions for Exercises 2.1 to 2.7 |
| `standalone_demos` | 1 and 2 | Plain C++ examples from the slides. These aren't ROS packages, and colcon skips the folder |

## Getting started

```bash
source /opt/ros/jazzy/setup.bash          # or humble; worth adding to ~/.bashrc

mkdir -p ~/ros2_ws/src && cd ~/ros2_ws/src
git clone <THIS REPO URL>

cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r -y
colcon build --symlink-install --cmake-args -DCMAKE_BUILD_TYPE=RelWithDebInfo
colcon test && colcon test-result --verbose

source ~/ros2_ws/install/setup.bash       # in every new terminal
```

Don't skip the build type. Without it CMake builds with no optimisation at all, and a control
loop can run around 10 times slower (Unit 1, slide 28).

## Unit 1: Build Tools

### diffbot_kinematics

The kinematics library. It has no ROS dependency, which is why its tests run in milliseconds.

* `kinematics.hpp` has `RobotGeometry`, `BodyTwist`, `WheelSpeeds` and `Pose2D`, plus the
  functions `to_wheel_speeds`, `to_body_twist` and `integrate`.
* `modern.hpp` has the C++20/23 examples from the end of Unit 1: a concept, `std::span`, and
  `std::expected` when building as C++23.

To run only its tests:

```bash
colcon test --packages-select diffbot_kinematics && colcon test-result --verbose
```

### diffbot_nodes

`wheel_speed_monitor` is Demo 1.4. It listens on `/odom` and prints the wheel speeds the library
calculates. You can try it without a simulator:

```bash
# terminal 1
ros2 run diffbot_nodes wheel_speed_monitor
# terminal 2
ros2 topic pub -r 2 /odom nav_msgs/msg/Odometry "{twist: {twist: {linear: {x: 0.2}, angular: {z: 0.5}}}}"
```

You should see:

```
[INFO] [wheel_speed_monitor]: v=0.200 m/s  w=0.500 rad/s  ->  left=4.85 rad/s  right=7.27 rad/s
```

The robot size can be changed with parameters, for example `--ros-args -p wheel_radius_m:=0.05`.

`odom_integrator` is the solution to Exercise 1.1, described next.

### Exercise 1.1

The exercise asks you to extend the packages above, so there's no separate `exercises_unit_1`
package. The solution is in the files you'd be editing:

| Step | Where |
|---|---|
| 1 and 2: `Pose2D` and `integrate()` | `diffbot_kinematics/include/diffbot_kinematics/kinematics.hpp` and `diffbot_kinematics/src/kinematics.cpp` |
| 3: the test | `IntegrateStraightLine` in `diffbot_kinematics/test/test_kinematics.cpp` |
| 4: the node | `diffbot_nodes/src/odom_integrator.cpp`, plus its lines in `diffbot_nodes/CMakeLists.txt` and `package.xml` |

To try it:

```bash
ros2 run diffbot_nodes odom_integrator
ros2 run teleop_twist_keyboard teleop_twist_keyboard     # in a second terminal
```

### Command line demos

These go with the slides where we build everything by hand, before CMake:

```bash
cd standalone_demos/unit1
bash compile_stages.sh           # preprocess, compile, assemble and link, one step at a time
bash diffbot_manual/build.sh     # the linker error, then static and shared libraries
cmake -S diffbot_cmake -B diffbot_cmake/build && cmake --build diffbot_cmake/build
```

## Unit 2: The STL Library

### diffbot_commander

The Unit 2 exercises drive the robot through this class, so they can stay focused on
containers. It creates its own node and spins it on a background thread.

```cpp
rclcpp::init(argc, argv);
{
  diffbot::Commander robot;               // waits up to 5 s for the first /odom
  robot.move_forward(2s);
  robot.turn(0.628, 2s);
  const auto [x, y, z] = robot.get_position();
  robot.stop_moving();
}                                         // robot and thread are stopped here
rclcpp::shutdown();
```

Parameters:

| Name | Default | What it does |
|---|---|---|
| `odom_topic` | `odom` | Odometry topic |
| `scan_topic` | `scan` | Laser topic |
| `cmd_vel_topic` | `cmd_vel` | Velocity command topic |
| `use_stamped_cmd_vel` | `false` | Send `TwistStamped` instead of `Twist` |
| `linear_speed_mps` | `0.2` | Speed used by `move_forward` and `move_backward` |
| `odom_wait_timeout_s` | `5.0` | How long the constructor waits for odometry |

A few things worth knowing:

* While the robot is moving, the command is sent again 10 times a second. Most velocity
  controllers stop the robot if commands stop arriving.
* If your simulator's controller expects `TwistStamped` (common with `ros2_control` on Jazzy),
  add `--ros-args -p use_stamped_cmd_vel:=true`.
* `get_laser_ranges()` returns an empty vector until the first scan comes in.
* Without a simulator, the constructor prints a warning after 5 s and positions read 0. The
  exercises still run, so it's a quick way to check your build.

### Exercises

| Exercise | Executable | Topic | Needs a robot |
|---|---|---|---|
| 2.1 | `robot_position_as_array` | `std::array`, structured bindings | yes |
| 2.2 | `robot_heading_as_vector` | `std::vector`, `reserve`, `std::ssize` | yes |
| 2.3 | `robot_heading_as_deque` | `std::deque`, `push_front` | yes |
| 2.4 | `laser_fields_as_list` | `std::list`, iterators, the `forward_list` trap | no |
| 2.5 | `path_coordinates_set` | `std::set`, `operator<=>`, floating-point keys | yes |
| 2.6 | `path_coordinates_map` | `std::map` and `std::unordered_map` | yes |
| 2.7 | `scan_analyser` | Capstone: algorithms, projections, lambdas, ranges | only `/scan` |

```bash
ros2 run exercises_unit_2 robot_heading_as_vector
ros2 run exercises_unit_2 scan_analyser --ros-args -p safety_distance_m:=0.8
```

### STL demos

One file per topic, numbered in slide order:

```bash
cd standalone_demos
bash build_all.sh --run                  # build and run all of them
CXX=g++ STD=c++23 bash build_all.sh      # try the C++23 parts (Jazzy's GCC 13)
```

| File | Slides | File | Slides |
|---|---|---|---|
| `01_array.cpp` | 6, 7 | `07_maps.cpp` | 26, 27 |
| `02_span.cpp` | 8 | `08_unordered_map.cpp` | 28, 29 |
| `03_vector.cpp` | 10 to 14 | `09_iterators.cpp` | 32 to 36 |
| `04_deque.cpp` | 17 | `10_algorithms.cpp` | 37 to 43 |
| `05_lists.cpp` | 18 | `11_lambdas.cpp` | 45 to 49 |
| `06_sets.cpp` | 21 to 25 | `12_ranges.cpp` | 50 to 53 |

## Compilers

The code builds without warnings (`-Wall -Wextra -Wpedantic`) on:

| Compiler | Where it comes from | C++20 | C++23 |
|---|---|---|---|
| GCC 11 | Humble default | yes | not tested |
| GCC 13 | Jazzy default | yes | yes |
| Clang 18 | Jazzy, `sudo apt install clang` | yes | yes |

The C++23 parts are switched on with feature-test macros rather than distro checks, so the same
code builds everywhere. To build the packages as C++23 on Jazzy:

```bash
colcon build --cmake-args -DCMAKE_CXX_STANDARD=23
```

## Notes for maintainers

* Add this repo's URL to Unit 2, slide 5.
* GitHub Actions (`.github/workflows/build.yml`) builds and tests everything on Humble and
  Jazzy on every push. For a status badge, add
  `![build](https://github.com/<OWNER>/<REPO>/actions/workflows/build.yml/badge.svg)` to the top
  of this file.

## Maintainer

Ali Pahlevani (a.pahlevani1998@gmail.com)

## License

Copyright 2026 Ali Pahlevani. Licensed under Apache 2.0, see [LICENSE](LICENSE).
