#include <gtest/gtest.h>
#include "transport/route_service.h"
#include "infra/logger.h"

using namespace urban_transport;

class RouteServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        urban_transport::Logger::get_instance().initialize();
        service.initialize(":memory:"); // Base de datos en memoria para pruebas
        
        // Crear esquema de prueba
        create_test_schema();
    }
    
    void TearDown() override {
        urban_transport::Logger::get_instance().shutdown();
    }
    
    void create_test_schema() {
        service.initialize(":memory:");
        
        // Crear tablas básicas para pruebas
        std::string sql = R"(
            CREATE TABLE stops (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                latitude REAL NOT NULL,
                longitude REAL NOT NULL
            );
            
            CREATE TABLE routes (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                transport_type TEXT NOT NULL
            );
            
            CREATE TABLE route_stops (
                route_id INTEGER,
                stop_id INTEGER,
                sequence INTEGER NOT NULL,
                PRIMARY KEY (route_id, stop_id)
            );
        )";
        
        // Ejecutar esquema (en una implementación real esto estaría en Database)
    }
    
    RouteService service;
};

TEST_F(RouteServiceTest, CreateRoute) {
    Route route(1, "Línea 1", "bus");
    EXPECT_TRUE(service.create_route(route));
    
    Route retrieved = service.get_route(1);
    EXPECT_EQ(retrieved.id, 1);
    EXPECT_EQ(retrieved.name, "Línea 1");
    EXPECT_EQ(retrieved.transport_type, "bus");
}

TEST_F(RouteServiceTest, GetNonExistentRoute) {
    Route route = service.get_route(999);
    EXPECT_EQ(route.id, 0); // ID por defecto para ruta no encontrada
    EXPECT_TRUE(route.name.empty());
}

TEST_F(RouteServiceTest, GetAllRoutes) {
    Route route1(1, "Línea 1", "bus");
    Route route2(2, "Línea 2", "metro");
    
    service.create_route(route1);
    service.create_route(route2);
    
    auto routes = service.get_all_routes();
    EXPECT_GE(routes.size(), 2);
}

TEST_F(RouteServiceTest, FindRoutesByType) {
    Route bus_route(1, "Bus 1", "bus");
    Route metro_route(2, "Metro 1", "metro");
    
    service.create_route(bus_route);
    service.create_route(metro_route);
    
    auto bus_routes = service.find_routes_by_type("bus");
    EXPECT_FALSE(bus_routes.empty());
    
    for (const auto& route : bus_routes) {
        EXPECT_EQ(route.transport_type, "bus");
    }
}

// Ejecutar tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}