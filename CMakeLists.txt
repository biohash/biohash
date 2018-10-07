cmake_minimum_required(VERSION 3.7)

project(BIOHASH VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")


add_subdirectory(src/cpp)


add_subdirectory(test/cpp)
