cmake_minimum_required(VERSION 3.8)
project(laser_reader)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

# laser_reader:     Exercise 5.3, smart pointers (the final version)
# laser_reader_raw: Exercises 5.1 and 5.2, raw pointers, kept for comparison
foreach(node laser_reader laser_reader_raw)
  add_executable(${node} src/${node}.cpp)
  target_link_libraries(${node} PRIVATE rclcpp::rclcpp ${sensor_msgs_TARGETS})
  install(TARGETS ${node} DESTINATION lib/${PROJECT_NAME})
endforeach()

ament_package()
