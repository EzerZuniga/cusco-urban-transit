include_guard(GLOBAL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(UT_ENABLE_STRICT_WARNINGS "Enable strict compiler warnings" ON)

function(ut_apply_target_defaults target_name)
    if(NOT TARGET ${target_name})
        message(FATAL_ERROR "ut_apply_target_defaults: target '${target_name}' does not exist")
    endif()

    target_compile_features(${target_name} PUBLIC cxx_std_17)

    if(MSVC)
        set_property(TARGET ${target_name} PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

        if(UT_ENABLE_STRICT_WARNINGS)
            target_compile_options(${target_name} PRIVATE /W4 /permissive- /utf-8)
        endif()
    else()
        if(UT_ENABLE_STRICT_WARNINGS)
            target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
        endif()

        target_compile_options(${target_name} PRIVATE
            $<$<CONFIG:Debug>:-O0;-g>
            $<$<NOT:$<CONFIG:Debug>>:-O2>)
    endif()
endfunction()
