cmake_minimum_required(VERSION 3.13)
project(http-server-starter-cpp)
set(CMAKE_CXX_STANDARD 20) # Enable the C++20 standard

# check all files in the src directory
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

add_executable(server ${SOURCE_FILES})
