cmake_minimum_required(VERSION 3.28)
project(AMZlab2)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(AMZlab2 main.cpp)

target_link_libraries(AMZlab2 PRIVATE sfml-graphics sfml-window sfml-system)