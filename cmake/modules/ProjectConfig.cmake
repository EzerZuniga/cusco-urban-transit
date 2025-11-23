
# Project-wide CMake configuration helpers for the project.
# This module centralizes common compiler flags, options and helper
# functions used by the repository's CMakeLists.

if(DEFINED PROJECT_CONFIG_CMAKE_INCLUDED)
	return()
endif()
set(PROJECT_CONFIG_CMAKE_INCLUDED TRUE)

# --- Project options -------------------------------------------------------
option(ENABLE_SANITIZERS "Enable Address/Undefined behaviour sanitizers when supported" OFF)
option(ENABLE_WARNINGS "Enable and elevate compiler warnings to errors where possible" ON)

if(NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard to use" FORCE)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# --- Compiler warning and sanitizer flags ---------------------------------
if(ENABLE_WARNINGS)
	if(MSVC)
		add_compile_options(/W4 /permissive- /utf-8)
	else()
		add_compile_options(-Wall -Wextra -Wpedantic)
		# Do not treat unused-parameter as error in some codepaths
		add_compile_options(-Wno-unused-parameter)
	endif()
endif()

if(ENABLE_SANITIZERS AND NOT MSVC)
	include(CheckCXXCompilerFlag)
	check_cxx_compiler_flag("-fsanitize=address" COMPILER_SUPPORTS_ASAN)
	if(COMPILER_SUPPORTS_ASAN)
		add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
		add_link_options(-fsanitize=address)
	endif()
	check_cxx_compiler_flag("-fsanitize=undefined" COMPILER_SUPPORTS_UBSAN)
	if(COMPILER_SUPPORTS_UBSAN)
		add_compile_options(-fsanitize=undefined)
		add_link_options(-fsanitize=undefined)
	endif()
endif()

# --- Find 3rd-party libraries ---------------------------------------------
# Prefer the project's `FindSQLite3.cmake` in this repo (cmake/modules).
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
find_package(SQLite3 REQUIRED)

# --- Helper: setup_common_target(target) ----------------------------------
# Apply common properties, include dirs and link libraries for a target.
function(setup_common_target target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "setup_common_target: target '${target}' not found")
	endif()

	# Ensure target uses the project C++ standard
	set_target_properties(${target} PROPERTIES
		CXX_STANDARD ${CMAKE_CXX_STANDARD}
		CXX_STANDARD_REQUIRED ON
		CXX_EXTENSIONS OFF)

	# Prefer project include dir if exists
	if(EXISTS "${PROJECT_SOURCE_DIR}/include")
		target_include_directories(${target} PRIVATE "${PROJECT_SOURCE_DIR}/include")
	endif()

	# Link SQLite3 (support both imported target and variables)
	if(TARGET SQLite::SQLite3)
		target_link_libraries(${target} PRIVATE SQLite::SQLite3)
	elseif(DEFINED SQLite3_LIBRARIES)
		target_include_directories(${target} PRIVATE ${SQLite3_INCLUDE_DIRS})
		target_link_libraries(${target} PRIVATE ${SQLite3_LIBRARIES})
	endif()

	if(MSVC)
		target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
	endif()
endfunction()

# --- Install dir defaults -------------------------------------------------
if(NOT DEFINED PROJECT_INSTALL_BINDIR)
	set(PROJECT_INSTALL_BINDIR "bin" CACHE STRING "Runtime install directory")
endif()
if(NOT DEFINED PROJECT_INSTALL_LIBDIR)
	set(PROJECT_INSTALL_LIBDIR "lib" CACHE STRING "Library install directory")
endif()

# Export compile commands by default (useful for clang-tidy, editors)
if(NOT CMAKE_EXPORT_COMPILE_COMMANDS)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands.json" FORCE)
endif()

message(STATUS "ProjectConfig: C++=${CMAKE_CXX_STANDARD}, ENABLE_WARNINGS=${ENABLE_WARNINGS}, ENABLE_SANITIZERS=${ENABLE_SANITIZERS}")
