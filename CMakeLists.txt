<<<<<<< HEAD
cmake_minimum_required(VERSION 3.10.0)

project(drone_control VERSION 0.1.0 LANGUAGES C CXX)

add_executable(drone_control main.cpp)
=======
cmake_minimum_required(VERSION 3.31.1)
project(drone_control VERSION 0.1.0 LANGUAGES C CXX)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")

find_package(box2d CONFIG REQUIRED)
add_executable(drone_control main.cpp)

target_link_libraries(drone_control PRIVATE box2d::box2d)
>>>>>>> a1593d9 (create initial structure)
