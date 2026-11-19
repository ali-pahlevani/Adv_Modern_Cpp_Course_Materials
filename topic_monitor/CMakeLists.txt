cmake_minimum_required(VERSION 3.8)
project(topic_monitor)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

# Templates are header-only: an INTERFACE library compiles nothing,
# it only hands its include path and dependencies to whoever links it
add_library(topic_monitor INTERFACE)
target_include_directories(topic_monitor INTERFACE include)
target_link_libraries(topic_monitor INTERFACE rclcpp::rclcpp)

add_executable(topic_monitor_node src/topic_monitor_node.cpp)
target_link_libraries(topic_monitor_node PRIVATE
  topic_monitor
  ${sensor_msgs_TARGETS} ${nav_msgs_TARGETS} ${std_msgs_TARGETS} ${geometry_msgs_TARGETS}
)
install(TARGETS topic_monitor_node DESTINATION lib/${PROJECT_NAME})

# Exercise 6.2, kept for comparison: the same monitor with full specializations
# instead of concepts and if constexpr
add_executable(topic_monitor_specialized_node src/topic_monitor_specialized_node.cpp)
target_link_libraries(topic_monitor_specialized_node PRIVATE
  topic_monitor
  ${sensor_msgs_TARGETS} ${nav_msgs_TARGETS} ${std_msgs_TARGETS}
)
install(TARGETS topic_monitor_specialized_node DESTINATION lib/${PROJECT_NAME})

ament_package()
