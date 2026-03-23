ut_require_gtest()

if(NOT TARGET GTest::gtest OR NOT TARGET GTest::gtest_main)
    message(STATUS "Skipping test target because GoogleTest is unavailable.")
    return()
endif()

add_executable(test_transport
    test_algorithms.cpp
    test_routes.cpp
    test_stops.cpp
)

target_link_libraries(test_transport
    PRIVATE
        urban_transport_core
        GTest::gtest
        GTest::gtest_main
)

file(TO_CMAKE_PATH "${CMAKE_CURRENT_BINARY_DIR}/runtime" UT_TEST_DB_DIR)
target_compile_definitions(test_transport
    PRIVATE UT_TEST_DB_DIR="${UT_TEST_DB_DIR}"
)

ut_apply_target_defaults(test_transport)

add_test(NAME TransportTests COMMAND test_transport)
