cmake_minimum_required(VERSION 3.17)
project(superluminal)

set(CMAKE_CXX_STANDARD 20)

add_executable(superluminal main.cpp httpserver.cpp httpserver.hpp utils.cpp utils.hpp)