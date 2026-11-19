cmake_minimum_required(VERSION 3.8)
project(my_robot_manager)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)     # Exercise 3.2
find_package(std_srvs REQUIRED)     # Exercise 3.5

# The class, compiled into a library
add_library(robot_manager STATIC src/robot_manager.cpp)
target_include_directories(robot_manager PUBLIC include)
# PUBLIC: the header includes rclcpp, nav_msgs and std_srvs
target_link_libraries(robot_manager PUBLIC
  rclcpp::rclcpp
  ${nav_msgs_TARGETS}
  ${std_srvs_TARGETS}
)

# The node, which uses the library
add_executable(robot_manager_node src/robot_manager_node.cpp)
target_link_libraries(robot_manager_node PRIVATE robot_manager rclcpp::rclcpp)

install(TARGETS robot_manager_node DESTINATION lib/${PROJECT_NAME})

ament_package()
