cmake_minimum_required(VERSION 3.19)
project(Blatt2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -fsanitize=address -DDEBUG")

add_executable(Applicable main.cpp)
add_executable(GoalTest goalTets.cpp)
add_executable(Series series.cpp)
add_executable(BFS bfs.cpp)
add_executable(GraphSearch graphSearch.cpp)
