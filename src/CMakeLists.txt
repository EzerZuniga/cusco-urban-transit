set(URBAN_TRANSPORT_CORE_SOURCES
    app/algorithms.cpp
    app/transport.cpp
    app/services/route_service.cpp
    app/services/stop_service.cpp
    app/services/trip_service.cpp
    core/graph.cpp
    infra/db.cpp
    infra/logging/logger.cpp
    infra/sqlite/connection_pool.cpp
    ${UT_DB_BACKEND_SOURCE}
)

add_library(urban_transport_core STATIC ${URBAN_TRANSPORT_CORE_SOURCES})
target_include_directories(urban_transport_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

ut_apply_target_defaults(urban_transport_core)
ut_link_database(urban_transport_core)
