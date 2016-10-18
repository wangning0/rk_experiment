cmake_minimum_required(VERSION 3.6)
project(rk_experiment_1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(rk_experiment_1 ${SOURCE_FILES})