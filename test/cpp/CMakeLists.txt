set(TEST_SOURCES
    test_base64.cpp
    test_http.cpp
    test_buffer.cpp
)

set(TEST_UTIL_SOURCES
    util/test_base.cpp
    util/test_runner.cpp
)

set(TEST_MAIN_SOURCES
    main.cpp
)

add_executable(BiohashTests ${TEST_UTIL_SOURCES} ${TEST_SOURCES} ${TEST_MAIN_SOURCES})
set_target_properties(BiohashTests PROPERTIES OUTPUT_NAME biohash-tests)
target_link_libraries(BiohashTests Biohash)
