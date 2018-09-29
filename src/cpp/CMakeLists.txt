set(BIOHASH_SOURCES
    biohash/foo.cpp
)

add_library(Biohash STATIC ${BIOHASH_SOURCES})
set_target_properties(Biohash PROPERTIES OUTPUT_NAME biohash)
target_include_directories(Biohash PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/cpp>)
