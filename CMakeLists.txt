cmake_minimum_required(VERSION 3.7)

project(BIOHASH VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(BearSSL STATIC IMPORTED)
set_property(TARGET BearSSL PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/external/BearSSL/inc)
set_property(TARGET BearSSL PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/BearSSL/build/libbearssl.a)

add_subdirectory(src/cpp)

add_subdirectory(test/cpp)
