cmake_minimum_required(VERSION 3.22)
project(exercises_unit_2 LANGUAGES CXX)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(diffbot_commander REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# One loop builds all the exercises (Unit 2, slide 54)
foreach(target
    robot_position_as_array     # Exercise 2.1, std::array
    robot_heading_as_vector     # Exercise 2.2, std::vector
    robot_heading_as_deque      # Exercise 2.3, std::deque
    laser_fields_as_list        # Exercise 2.4, std::list and iterators
    path_coordinates_set        # Exercise 2.5, std::set and operator<=>
    path_coordinates_map        # Exercise 2.6, std::map and std::unordered_map
    scan_analyser)              # Exercise 2.7, the capstone

  add_executable(${target} src/${target}.cpp)
  target_link_libraries(${target} PRIVATE
    rclcpp::rclcpp
    ${sensor_msgs_TARGETS}
    diffbot_commander::diffbot_commander
  )
  install(TARGETS ${target} DESTINATION lib/${PROJECT_NAME})
endforeach()

ament_package()
