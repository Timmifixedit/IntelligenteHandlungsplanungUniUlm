cmake_minimum_required(VERSION 3.19)
project(Blatt3)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic -fsanitize=address")

add_executable(Blatt3 main.cpp VariablePredicate.cpp State.cpp Operator.cpp util.cpp)