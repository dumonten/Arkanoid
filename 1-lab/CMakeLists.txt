cmake_minimum_required(VERSION 3.24)
project(1_lab)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(1_lab main.cpp game_classes.cpp)

target_link_libraries(1_lab sfml-graphics sfml-audio)