cmake_minimum_required(VERSION 3.20)
project(build_your_stl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()
add_subdirectory(infrastructure/test)

add_subdirectory(exercises)
