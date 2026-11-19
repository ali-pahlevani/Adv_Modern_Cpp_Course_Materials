cmake_minimum_required(VERSION 3.8)
project(unit8_exceptions)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

set(CMAKE_CXX_STANDARD 20)

add_library(turn_validation INTERFACE)                         # the header from Exercise 8.1
target_include_directories(turn_validation INTERFACE include)

add_executable(validate_turn src/validate_turn.cpp)
target_link_libraries(validate_turn PRIVATE turn_validation)
add_executable(wheel_config src/wheel_config.cpp)
target_link_libraries(wheel_config PRIVATE rclcpp::rclcpp ${std_msgs_TARGETS})
add_executable(turn_commander src/turn_commander.cpp)
target_link_libraries(turn_commander PRIVATE
  turn_validation rclcpp::rclcpp ${std_msgs_TARGETS} ${geometry_msgs_TARGETS})

install(TARGETS validate_turn wheel_config turn_commander DESTINATION lib/${PROJECT_NAME})

ament_package()
