cmake_minimum_required(VERSION 3.15)
project(UrbanTransportSystem VERSION 1.0.0 LANGUAGES CXX)

# Configuración de C++ estándar
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Directorios de inclusión
include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Configuración de compilación
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-g -O0 -Wall -Wextra -pedantic)
else()
    add_compile_options(-O2 -Wall)
endif()

# Buscar SQLite3
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLite3 REQUIRED sqlite3)

# Buscar GoogleTest
find_package(GTest REQUIRED)

# Archivos fuente principales
set(SOURCES
    src/main.cpp
    src/app/transport.cpp
    src/app/algorithms.cpp
    src/app/services/route_service.cpp
    src/app/services/stop_service.cpp
    src/app/services/trip_service.cpp
    src/infra/db.cpp
    src/infra/sqlite/sqlite_wrapper.cpp
    src/infra/sqlite/connection_pool.cpp
    src/infra/logging/logger.cpp
    src/core/graph.cpp
)

# Ejecutable principal
add_executable(urban-transport-system ${SOURCES})
target_link_libraries(urban-transport-system ${SQLite3_LIBRARIES})
target_include_directories(urban-transport-system PRIVATE ${SQLite3_INCLUDE_DIRS})

# Tests
enable_testing()
add_executable(test_transport
    tests/test_routes.cpp
    tests/test_stops.cpp
    tests/test_algorithms.cpp
    src/app/transport.cpp
    src/app/algorithms.cpp
    src/app/services/route_service.cpp
    src/app/services/stop_service.cpp
    src/infra/db.cpp
    src/infra/sqlite/sqlite_wrapper.cpp
    src/infra/logging/logger.cpp
    src/core/graph.cpp
)
target_link_libraries(test_transport GTest::gtest GTest::gtest_main ${SQLite3_LIBRARIES})
target_include_directories(test_transport PRIVATE ${SQLite3_INCLUDE_DIRS})

add_test(NAME TransportTests COMMAND test_transport)