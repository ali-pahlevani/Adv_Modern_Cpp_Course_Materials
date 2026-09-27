cmake_minimum_required(VERSION 3.22)
project(diffbot_commander LANGUAGES CXX)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_library(diffbot_commander SHARED src/commander.cpp)
add_library(diffbot_commander::diffbot_commander ALIAS diffbot_commander)

target_compile_features(diffbot_commander PUBLIC cxx_std_20)

target_include_directories(diffbot_commander PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

# The public header doesn't include any ROS headers (pimpl), so ROS can be a
# PRIVATE dependency (Unit 1, slide 52).
target_link_libraries(diffbot_commander PRIVATE
  rclcpp::rclcpp
  ${nav_msgs_TARGETS}
  ${sensor_msgs_TARGETS}
  ${geometry_msgs_TARGETS}
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(TARGETS diffbot_commander
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)
# Packages linking to this library need these found too, to resolve its own dependencies.
ament_export_dependencies(rclcpp nav_msgs sensor_msgs geometry_msgs)

ament_package()
