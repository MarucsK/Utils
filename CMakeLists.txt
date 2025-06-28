cmake_minimum_required(VERSION 3.15)
project(Utils VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_library(UTILS INTERFACE)
target_include_directories(UTILS INTERFACE include)

add_subdirectory(test)