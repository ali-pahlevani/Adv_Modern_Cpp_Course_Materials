cmake_minimum_required(VERSION 3.22)
project(diffbot_nodes LANGUAGES CXX)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(diffbot_kinematics REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Demo 1.4 (Unit 1), prints wheel speeds for each /odom message
add_executable(wheel_speed_monitor src/wheel_speed_monitor.cpp)
target_link_libraries(wheel_speed_monitor PRIVATE
  rclcpp::rclcpp
  ${nav_msgs_TARGETS}
  diffbot_kinematics::diffbot_kinematics
)

# Exercise 1.1 (Unit 1), estimates the pose from /cmd_vel
add_executable(odom_integrator src/odom_integrator.cpp)
target_link_libraries(odom_integrator PRIVATE
  rclcpp::rclcpp
  ${geometry_msgs_TARGETS}
  diffbot_kinematics::diffbot_kinematics
)

# lib/<package>/ is where `ros2 run` looks (Unit 1, slide 77)
install(TARGETS wheel_speed_monitor odom_integrator
  DESTINATION lib/${PROJECT_NAME})

ament_package()
