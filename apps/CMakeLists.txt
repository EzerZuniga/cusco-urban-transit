add_executable(urban-transport-system
    cli/main.cpp
)

target_link_libraries(urban-transport-system
    PRIVATE urban_transport_core
)

ut_apply_target_defaults(urban-transport-system)
