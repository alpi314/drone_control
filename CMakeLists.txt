cmake_minimum_required(VERSION 3.16)
project(drone_control VERSION 0.1.0 LANGUAGES CXX)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(box2d CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

add_executable(drone_control main.cpp)

target_link_libraries(drone_control
  PRIVATE
    box2d::box2d
    SFML::Network SFML::Graphics SFML::Window SFML::Audio SFML::System
)
