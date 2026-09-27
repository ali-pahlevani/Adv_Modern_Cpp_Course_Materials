# Unit 1, Demo 1.2 (slide 53). The same build as the manual commands, done by CMake.
#   cmake -B build -S . && cmake --build build && ./build/diffbot_demo
cmake_minimum_required(VERSION 3.22)
project(diffbot_cmake LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)             # -std=c++20, not -std=gnu++20
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)     # compile_commands.json for clangd (slide 56)

add_library(diffbot_kinematics src/kinematics.cpp)
target_include_directories(diffbot_kinematics PUBLIC include)
target_compile_features(diffbot_kinematics PUBLIC cxx_std_20)

add_executable(diffbot_demo src/main.cpp)
target_link_libraries(diffbot_demo PRIVATE diffbot_kinematics)
