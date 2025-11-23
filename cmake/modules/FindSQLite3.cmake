# FindSQLite3.cmake - Find SQLite3 library
# Adapted from CMake's standard FindSQLite3.cmake

find_path(SQLite3_INCLUDE_DIR
    NAMES sqlite3.h
    PATHS
        /usr/include
        /usr/local/include
        /opt/local/include
)

find_library(SQLite3_LIBRARY
    NAMES sqlite3 sqlite3-0
    PATHS
        /usr/lib
        /usr/local/lib
        /opt/local/lib
)

if(SQLite3_INCLUDE_DIR AND SQLite3_LIBRARY)
    set(SQLite3_FOUND TRUE)
    set(SQLite3_LIBRARIES ${SQLite3_LIBRARY})
    set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR})
endif()

if(SQLite3_FOUND)
    if(NOT SQLite3_FIND_QUIETLY)
        message(STATUS "Found SQLite3: ${SQLite3_LIBRARY}")
    endif()
else()
    if(SQLite3_FIND_REQUIRED)
        message(FATAL_ERROR "Could not find SQLite3")
    endif()
endif()

mark_as_advanced(SQLite3_INCLUDE_DIR SQLite3_LIBRARY)