cmake_minimum_required(VERSION 3.22)
project(diffbot_kinematics LANGUAGES CXX)

find_package(ament_cmake REQUIRED)

# C++20 by default. Override with:  colcon build --cmake-args -DCMAKE_CXX_STANDARD=23
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# SHARED: ROS 2 composition and pluginlib both require .so libraries (Unit 1, slide 31)
add_library(diffbot_kinematics SHARED src/kinematics.cpp)

# Same name downstream packages see after ament_export_targets (Unit 1, slide 64)
add_library(diffbot_kinematics::diffbot_kinematics ALIAS diffbot_kinematics)

target_compile_features(diffbot_kinematics PUBLIC cxx_std_20)

# Headers live in two places with different paths (Unit 1, slide 65)
target_include_directories(diffbot_kinematics PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})

install(TARGETS diffbot_kinematics
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

ament_export_targets(export_${PROJECT_NAME} HAS_LIBRARY_TARGET)

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_kinematics test/test_kinematics.cpp)
  # Plain signature on purpose: ament_add_gtest uses the plain form internally,
  # and CMake forbids mixing plain and keyword (PRIVATE/PUBLIC) calls on one target.
  target_link_libraries(test_kinematics diffbot_kinematics)
endif()

ament_package()
