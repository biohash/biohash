set(BIOHASH_SOURCES
    biohash/log.cpp
    biohash/time.cpp
    biohash/base64.cpp
    biohash/buffer.cpp
    biohash/json.cpp
    biohash/http.cpp
    biohash/websocket.cpp
)

add_library(Biohash STATIC ${BIOHASH_SOURCES})
set_target_properties(Biohash PROPERTIES OUTPUT_NAME biohash)
target_include_directories(Biohash PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src/cpp>)
target_link_libraries(Biohash BearSSL)
