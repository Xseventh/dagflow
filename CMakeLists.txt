cmake_minimum_required(VERSION 3.12)
project(dagflow)

set(CMAKE_CXX_STANDARD 17)
file(GLOB_RECURSE src ./*.cpp ./*.h)
include_directories(..)
add_executable(test test.cpp)

add_executable(dag_flower_info_unittest dagflower/dagflower_info/dag_flower_info_unittest.cpp)
add_executable(dag_flower_unittest dag_flower_unittest.cpp)