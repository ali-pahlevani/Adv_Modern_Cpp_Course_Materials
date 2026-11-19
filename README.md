# Advanced Modern C++ for Robotics & ROS 2

Code for the course. All the examples use the same robot, diffbot: a small two-wheeled
differential drive robot with a wheel radius of 0.033 m and a wheel separation of 0.160 m.

Written for ROS 2 Humble (Ubuntu 22.04) and Jazzy (Ubuntu 24.04). The code is C++20, with a
few optional C++23 parts that switch on when the compiler supports them.

The repo covers all eight units.

## What's in the repo

| Folder | Unit | Contents |
|---|---|---|
| `diffbot_kinematics` | 1 | Kinematics library in plain C++20 (no ROS), with GTest tests |
| `diffbot_nodes` | 1 | ROS 2 nodes that use the library, including the Exercise 1.1 solution |
| `diffbot_commander` | 2 | Helper class for driving the robot from a normal `main()` |
| `exercises_unit_2` | 2 | Solutions for Exercises 2.1 to 2.7 |
| `my_robot_manager` | 3 | Solutions for Exercises 3.1 to 3.6 |
| `robot_manager_oop` | 4 | Solutions for Exercises 4.1, 4.3, 4.4 and 4.5 |
| `laser_reader` | 5 | Solutions for Exercises 5.1 to 5.3 |
| `topic_monitor` | 6 | Solutions for Exercises 6.1 to 6.3 |
| `unit7_concurrency` | 7 | Demo 7.3 and the Exercise 7.2 solution |
| `unit8_exceptions` | 8 | Demo 8.3 and the solutions for Exercises 8.1 and 8.2 |
| `standalone_demos` | 1 to 8 | Plain C++ examples from the slides, plus Exercises 3.7 and 7.1. These aren't ROS packages, and colcon skips the folder |

The package names are the ones the slides use, so every `ros2 run` command on the slides works
with this repo.

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

If you are following the slides and creating the packages yourself, build this repo in a
separate workspace. Two packages with the same name in one workspace stop colcon.

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

| File | Slides | File | Slides |
|---|---|---|---|
| `01_array.cpp` | 6, 7 | `07_maps.cpp` | 26, 27 |
| `02_span.cpp` | 8 | `08_unordered_map.cpp` | 28, 29 |
| `03_vector.cpp` | 10 to 14 | `09_iterators.cpp` | 32 to 36 |
| `04_deque.cpp` | 17 | `10_algorithms.cpp` | 37 to 43 |
| `05_lists.cpp` | 18 | `11_lambdas.cpp` | 45 to 49 |
| `06_sets.cpp` | 21 to 25 | `12_ranges.cpp` | 50 to 53 |

## Unit 3: Classes and Objects

### my_robot_manager

The `RobotManager` class after Exercises 3.1 to 3.6. Each exercise grows the same files,
so the package holds the final version:

| Exercise | Slides | What it adds |
|---|---|---|
| 3.1 | 13 to 15 | The class as a `STATIC` library, and a node that uses it |
| 3.2 | 16 to 18 | Private members, and a constructor that subscribes to odometry |
| 3.3 | 21 | The static `robot_count()`, and a second robot |
| 3.4 | 24 | `const` getters, and `log_summary()` in the node file |
| 3.5 | 28 to 30 | The `log_current_position` service, and deleted copy operations |
| 3.6 | 36 | The delegating constructor |

```bash
ros2 run my_robot_manager robot_manager_node
ros2 topic pub -r 2 /diffbot_1/odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
ros2 service call /diffbot_1/log_current_position std_srvs/srv/Trigger
```

### Exercise 3.7

Plain C++, in `standalone_demos/unit3/exercise_37`:

| File | What it is |
|---|---|
| `occupancy_grid.cpp` | The starter. It ends with a double free, on purpose |
| `occupancy_grid_rule_of_five.cpp` | Part I: the four copy and move operations written by hand |
| `occupancy_grid_rule_of_zero.cpp` | Part II: a `std::vector` member, and none of them |

```bash
cd standalone_demos/unit3/exercise_37
clang++ -std=c++20 -Wall -Wextra -fsanitize=address,undefined occupancy_grid.cpp -o occupancy_grid && ./occupancy_grid
```

### Demos

| File | Slide | File | Slide |
|---|---|---|---|
| `demo_31.cpp` | 4 | `demo_38.cpp` | 32 |
| `demo_32.cpp` | 5 | `demo_39.cpp` | 33 |
| `demo_33.cpp` | 10 | `demo_310.cpp` | 35 |
| `demo_34.cpp` | 20 | `demo_311.cpp` | 38 |
| `demo_35.cpp` | 23 | `demo_312.cpp` | 41 |
| `demo_36.cpp` | 25 | `demo_313.cpp` | 42 |
| `demo_37.cpp` | 26 | | |

## Unit 4: Object-Oriented Programming

### robot_manager_oop

Exercises 4.1, 4.3, 4.4 and 4.5 grow this one package. Exercise 4.2 is a quiz on paper.

| Exercise | Slides | Where |
|---|---|---|
| 4.1 | 3 to 5 | `computer_unit.hpp` (given), `robot_manager.hpp` and `.cpp`, `robot_manager_node.cpp` |
| 4.3 | 9, 10 | `robot_manager_base.hpp` and `.cpp`, `mobile_robot_manager.hpp` and `.cpp`, `mobile_robot_manager_node.cpp` |
| 4.4 | 14 | `display_robot_details()` in the base and the mobile manager |
| 4.5 | 16 | `display_robot_details()` becomes pure virtual in `robot_manager_base.hpp` |

Exercise 4.5 makes `RobotManagerBase` abstract, so its test node from Exercise 4.3,
`robot_manager_base_node`, can't exist any more and isn't in the package.

```bash
ros2 run robot_manager_oop mobile_robot_manager_node
ros2 topic pub -r 2 /odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
ros2 service call /robot_manager_output std_srvs/srv/SetBool "{data: true}"
```

### Demos

| File | Slide | File | Slide |
|---|---|---|---|
| `demo_41.cpp` | 2 | `demo_45.cpp` | 13 |
| `demo_42.cpp` | 7 | `demo_46.cpp` | 15 |
| `demo_43.cpp` | 11 | `demo_47.cpp` | 17 |
| `demo_44.cpp` | 11 | `demo_48.cpp` | 18 |

## Unit 5: Pointers and References

### laser_reader

Exercise 5.3 rewrites the code from Exercises 5.1 and 5.2, so both versions are here:

| Executable | Exercises | Slides | What it shows |
|---|---|---|---|
| `laser_reader_raw` | 5.1 and 5.2 | 4, 7 | Raw buffers with `new[]` and `delete[]`, an array of pointers, and a `const &` |
| `laser_reader` | 5.3 | 9 | The same node with smart pointers only, and no destructor |

```bash
ros2 run laser_reader laser_reader
ros2 topic pub -r 2 /scan sensor_msgs/msg/LaserScan "{angle_min: -1.5708, angle_increment: 0.7854, range_min: 0.1, range_max: 10.0, ranges: [1.0, 1.5, 2.0, 2.5]}"
```

### Demos

| File | Slide |
|---|---|
| `demo_51.cpp` | 2 |
| `demo_52.cpp` | 3 |
| `demo_53.cpp` | 5 |
| `demo_54.cpp` | 6 |
| `demo_55.cpp` | 8 |

## Unit 6: Templates and Lambda Expressions

### topic_monitor

One class template that monitors any topic. Exercise 6.3 replaces the specializations from
Exercise 6.2, so both versions are here:

| Executable | Header | Exercises | Slides |
|---|---|---|---|
| `topic_monitor_node` | `topic_monitor.hpp` and `moving_average.hpp` | 6.1 and 6.3 | 6, 7, 13, 14 |
| `topic_monitor_specialized_node` | `topic_monitor_specialized.hpp` | 6.1 and 6.2 | 6, 7, 9 |

```bash
ros2 run topic_monitor topic_monitor_node
ros2 topic pub -r 1 /scan sensor_msgs/msg/LaserScan "{angle_min: -1.5708, angle_increment: 0.7854, range_min: 0.1, range_max: 10.0, ranges: [1.0, 1.5, 2.0, 2.5]}"
ros2 topic pub -r 1 /amcl_pose geometry_msgs/msg/PoseWithCovarianceStamped "{pose: {pose: {position: {x: 1.0, y: 2.0}}}}"
```

### Demos

| File | Slide | File | Slide |
|---|---|---|---|
| `demo_61.cpp` | 2 | `demo_65.cpp` | 8 |
| `demo_62/` and `demo_62_fixed/` | 3 | `demo_66.cpp` | 10 |
| `demo_63.cpp` and `demo_63_moving_average.hpp` | 4 | `demo_67.cpp` | 11 |
| `demo_64.cpp` | 5 | `demo_68.cpp` | 12 |

`demo_62` has three files and is meant to fail at the link step. `demo_62_fixed` is the same
code with the explicit instantiation from the slide:

```bash
cd standalone_demos/unit6
g++ -std=c++20 demo_62/main.cpp demo_62/printer.cpp -o demo_62                     # undefined reference
g++ -std=c++20 demo_62_fixed/main.cpp demo_62_fixed/printer.cpp -o demo_62_fixed   # builds
```

## Unit 7: Concurrency

### unit7_concurrency

| Executable | What it is | Slide |
|---|---|---|
| `control_loop` | Demo 7.3: run it as `single`, `multi` and `groups` | 5 |
| `odom_watch` | Exercise 7.2: two callback groups, two threads and a mutex | 6 |
| `odom_watch_starter` | Exercise 7.2, step 3: the version that stalls | 6 |

```bash
ros2 run unit7_concurrency control_loop single     # then: multi, groups

ros2 topic pub -r 20 /odom nav_msgs/msg/Odometry "{pose: {pose: {position: {x: 0.5, y: 0.2}}}}"
ros2 run unit7_concurrency odom_watch_starter      # max gap about 300 ms
ros2 run unit7_concurrency odom_watch              # max gap about 50 ms
```

### Demos and Exercise 7.1

| File | What it is | Slide |
|---|---|---|
| `demo_71_threads.cpp` | Demo 7.1: `std::thread`, `std::jthread`, `std::async` | 2 |
| `demo_72_shared.cpp` | Demo 7.2: a data race, a mutex and an atomic | 3 |
| `exercise_71_pose_store.cpp` | Exercise 7.1: a pose shared by two threads | 4 |

Run the data race demo under ThreadSanitizer as well. Build it without `-O2`, or the race can
go unreported:

```bash
cd standalone_demos/unit7
g++ -std=c++20 -g -fsanitize=thread demo_72_shared.cpp -o demo_72_tsan && ./demo_72_tsan
```

## Unit 8: Exception Handling

### unit8_exceptions

| Executable | What it is | Slide |
|---|---|---|
| `validate_turn` | Exercise 8.1: reads turn velocities from the keyboard | 4 |
| `wheel_config` | Demo 8.3: a parameter of the wrong type, and a callback that throws | 5 |
| `turn_commander` | Exercise 8.2: turns text commands into `cmd_vel`, and survives bad input | 6 |

`validate_turn` and `turn_commander` share `include/unit8_exceptions/turn_validation.hpp`
through an `INTERFACE` library.

```bash
ros2 run unit8_exceptions wheel_config --ros-args -p wheel_radius:=small
ros2 run unit8_exceptions turn_commander
ros2 topic pub --once /turn_cmd std_msgs/msg/String "{data: '-0.5'}"
```

### Demos

| File | What it is | Slide |
|---|---|---|
| `demo_81_try_catch.cpp` | Demo 8.1: `try`, `throw`, `catch` and stack unwinding | 2 |
| `demo_82_custom.cpp` | Demo 8.2: your own exception classes | 3 |
| `demo_84_expected.cpp` | Demo 8.4: `std::expected` (C++23) | 7 |

`demo_84_expected.cpp` needs GCC 13 or newer, so it builds on Jazzy but not on Humble:

```bash
g++ -std=c++23 demo_84_expected.cpp -o demo_84 && ./demo_84
```

## Standalone demos

```bash
cd standalone_demos
bash build_all.sh --run                  # build and run all of them
CXX=g++ STD=c++23 bash build_all.sh      # try the C++23 parts (Jazzy's GCC 13)
```

The demo files are exactly as on the slides, so line numbers in compiler errors and sanitizer
reports match the slides too. Some of them go wrong on purpose, and `build_all.sh` counts that
as a pass:

| File | What happens |
|---|---|
| `unit3/demo_32.cpp` | Doesn't compile: private members |
| `unit3/demo_36.cpp` | Clang warns about the self assignment |
| `unit3/demo_38.cpp` | Crashes with a double free |
| `unit3/exercise_37/occupancy_grid.cpp` | Crashes with a double free (the starter) |
| `unit4/demo_45.cpp` | Clang warns about the missing virtual destructor |
| `unit5/demo_54.cpp` | Reads through a dangling pointer, so it prints a random number |
| `unit6/demo_62/` | Doesn't link: the template is defined in a `.cpp` file |
| `unit6/demo_68.cpp` | Warns about the implicit capture of `this`, and reads a dangling reference |
| `unit7/demo_72_shared.cpp` | Has a data race, so the unsafe count changes from run to run |
| `unit8/demo_84_expected.cpp` | Skipped unless the compiler has `std::expected` |

## Compilers

Every package builds without warnings (`-Wall -Wextra -Wpedantic`), and every standalone demo
outside the table above builds with `-Werror` too, on:

| Compiler | Where it comes from | C++20 | C++23 |
|---|---|---|---|
| GCC 11 | Humble default | yes | not tested |
| GCC 13 | Jazzy default | yes | yes |
| Clang 18 | Jazzy, `sudo apt install clang` | yes | yes, except `std::expected` |

Clang 18 uses GCC 13's standard library on Ubuntu 24.04, and that combination has no
`std::expected`, so Demo 8.4 needs GCC.

The C++23 parts of Units 1 and 2 are switched on with feature-test macros rather than distro
checks, so the same code builds everywhere. To build those packages as C++23 on Jazzy:

```bash
colcon build --cmake-args -DCMAKE_CXX_STANDARD=23
```

The Unit 3 to 8 packages set C++20 in their `CMakeLists.txt`, the same way the slides do.

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
