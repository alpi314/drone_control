cmake_minimum_required(VERSION 3.30)
project(MyCMakeProject VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

add_executable(DroneControl main.cpp)

find_package(box2d CONFIG REQUIRED)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

target_link_libraries(DroneControl 
    PRIVATE 
        box2d::box2d
        SFML::Network 
        SFML::Graphics 
        SFML::Window 
        SFML::Audio
        SFML::System)

set_property(TARGET DroneControl PROPERTY CXX_STANDARD 20)