cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0135 NEW)
project(UrbanTransportSystem VERSION 1.0.0 LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

include(cmake/ProjectOptions.cmake)
include(cmake/Dependencies.cmake)

add_subdirectory(src)
add_subdirectory(apps)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
