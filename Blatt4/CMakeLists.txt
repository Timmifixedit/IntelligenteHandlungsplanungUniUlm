cmake_minimum_required(VERSION 3.19)
project(Blatt4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -fsanitize=address -DDEBUG")

add_executable(Blatt4 main.cpp)
add_executable(Blatt4Graph graph.cpp)
