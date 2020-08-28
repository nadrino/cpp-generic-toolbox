cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

project(cpp-generic-toolbox)

# This logger is written considering C++11
set (CMAKE_CXX_STANDARD 11)

# Include the header folder
include_directories(./include)

add_executable(GenericToolboxExample example/main.cpp)
