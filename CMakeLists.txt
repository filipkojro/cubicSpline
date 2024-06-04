cmake_minimum_required(VERSION 3.28)
project(cubicSpline)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(cubicSpline main.cpp)

target_link_libraries(cubicSpline PRIVATE sfml-graphics sfml-window sfml-system)
