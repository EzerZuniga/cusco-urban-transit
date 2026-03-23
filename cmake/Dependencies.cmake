include_guard(GLOBAL)

include(FetchContent)

option(USE_POSTGRES "Use PostgreSQL instead of SQLite" OFF)
option(UT_FETCH_MISSING_TEST_DEPS "Allow downloading missing test dependencies" OFF)

set(UT_DB_LIBRARIES "")
set(UT_DB_INCLUDE_DIRS "")
set(UT_DB_BACKEND_SOURCE "")

if(USE_POSTGRES)
    find_package(PostgreSQL REQUIRED)

    if(DEFINED PostgreSQL_LIBRARIES)
        set(UT_DB_LIBRARIES ${PostgreSQL_LIBRARIES})
    else()
        set(UT_DB_LIBRARIES ${PostgreSQL_LIBRARY})
    endif()

    if(DEFINED PostgreSQL_INCLUDE_DIRS)
        set(UT_DB_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIRS})
    else()
        set(UT_DB_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIR})
    endif()

    set(UT_DB_BACKEND_SOURCE "${PROJECT_SOURCE_DIR}/src/infra/postgres_wrapper.cpp")
else()
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        pkg_check_modules(SQLite3 QUIET sqlite3)
    endif()

    if(SQLite3_FOUND)
        set(UT_DB_LIBRARIES ${SQLite3_LIBRARIES})
        set(UT_DB_INCLUDE_DIRS ${SQLite3_INCLUDE_DIRS})
    else()
        find_package(SQLite3 QUIET)
        if(TARGET SQLite::SQLite3)
            set(UT_DB_LIBRARIES SQLite::SQLite3)
        else()
            if(DEFINED SQLite3_LIBRARIES)
                set(UT_DB_LIBRARIES ${SQLite3_LIBRARIES})
            endif()
            if(DEFINED SQLite3_INCLUDE_DIRS)
                set(UT_DB_INCLUDE_DIRS ${SQLite3_INCLUDE_DIRS})
            endif()
        endif()
    endif()

    if(NOT UT_DB_LIBRARIES)
        if(EXISTS "${PROJECT_SOURCE_DIR}/third_party/sqlite/sqlite3.c")
            message(STATUS "Using vendored SQLite amalgamation at third_party/sqlite/")
            set(SQLITE_AMALGAMATION_DIR "${PROJECT_SOURCE_DIR}/third_party/sqlite")
        else()
            message(STATUS "SQLite not found. Fetching amalgamation via FetchContent.")
            FetchContent_Declare(
                sqlite_amalgamation
                URL https://www.sqlite.org/2024/sqlite-amalgamation-3410200.zip
                DOWNLOAD_EXTRACT_TIMESTAMP TRUE
            )
            FetchContent_MakeAvailable(sqlite_amalgamation)
            set(SQLITE_AMALGAMATION_DIR "${sqlite_amalgamation_SOURCE_DIR}")
        endif()

        add_library(sqlite3_embedded STATIC "${SQLITE_AMALGAMATION_DIR}/sqlite3.c")
        target_include_directories(sqlite3_embedded PUBLIC "${SQLITE_AMALGAMATION_DIR}")
        target_compile_definitions(sqlite3_embedded PUBLIC SQLITE_THREADSAFE=1 SQLITE_ENABLE_JSON1)

        set(UT_DB_LIBRARIES sqlite3_embedded)
        set(UT_DB_INCLUDE_DIRS "${SQLITE_AMALGAMATION_DIR}")
    endif()

    set(UT_DB_BACKEND_SOURCE "${PROJECT_SOURCE_DIR}/src/infra/sqlite/sqlite_wrapper.cpp")
endif()

function(ut_link_database target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "ut_link_database: target '${target_name}' does not exist")
    endif()

    target_link_libraries(${target_name} PRIVATE ${UT_DB_LIBRARIES})

    if(UT_DB_INCLUDE_DIRS)
        target_include_directories(${target_name} PRIVATE ${UT_DB_INCLUDE_DIRS})
    endif()

    if(USE_POSTGRES)
        target_compile_definitions(${target_name} PRIVATE USE_POSTGRES)
    endif()
endfunction()

function(ut_require_gtest)
    find_package(GTest QUIET)
    if(GTest_FOUND)
        return()
    endif()

    if(NOT UT_FETCH_MISSING_TEST_DEPS)
        message(WARNING
            "GoogleTest not found and UT_FETCH_MISSING_TEST_DEPS=OFF. "
            "Test targets will be skipped."
        )
        return()
    endif()

    if(MSVC)
        set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT for GoogleTest" FORCE)
    endif()

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)
endfunction()
