set(TEST_SOURCES
    test_http.cpp
)

set(TEST_UTIL_SOURCES
    util/test_instance.cpp
)

set(TEST_MAIN_SOURCES
    main.cpp
)

add_executable(BiohashTests ${TEST_UTIL_SOURCES} ${TEST_SOURCES} ${TEST_MAIN_SOURCES})
set_target_properties(BiohashTests PROPERTIES OUTPUT_NAME biohash-tests)
