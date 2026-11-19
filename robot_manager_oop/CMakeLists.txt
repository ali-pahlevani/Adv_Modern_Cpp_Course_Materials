cmake_minimum_required(VERSION 3.8)
project(robot_manager_oop)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_srvs REQUIRED)

add_library(robot_managers STATIC
  src/robot_manager.cpp
  src/robot_manager_base.cpp
  src/mobile_robot_manager.cpp
)
target_include_directories(robot_managers PUBLIC include)
target_link_libraries(robot_managers PUBLIC rclcpp::rclcpp ${nav_msgs_TARGETS} ${std_srvs_TARGETS})

foreach(node robot_manager_node mobile_robot_manager_node)
  add_executable(${node} src/${node}.cpp)
  target_link_libraries(${node} PRIVATE robot_managers)
  install(TARGETS ${node} DESTINATION lib/${PROJECT_NAME})
endforeach()

ament_package()
