cmake_minimum_required(VERSION 3.8)
project(unit7_concurrency)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)

set(CMAKE_CXX_STANDARD 20)                                     # C++20 for the targets below
add_executable(control_loop src/control_loop.cpp)
target_link_libraries(control_loop PRIVATE rclcpp::rclcpp)
add_executable(odom_watch src/odom_watch.cpp)                  # Exercise 7.2
target_link_libraries(odom_watch PRIVATE rclcpp::rclcpp ${nav_msgs_TARGETS})
install(TARGETS control_loop odom_watch DESTINATION lib/${PROJECT_NAME})

# Exercise 7.2, step 3: the version that stalls (one thread, no groups, no mutex)
add_executable(odom_watch_starter src/odom_watch_starter.cpp)
target_link_libraries(odom_watch_starter PRIVATE rclcpp::rclcpp ${nav_msgs_TARGETS})
install(TARGETS odom_watch_starter DESTINATION lib/${PROJECT_NAME})

ament_package()
