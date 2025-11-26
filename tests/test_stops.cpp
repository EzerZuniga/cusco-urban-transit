#include <gtest/gtest.h>
#include <cstdio>
#include "transport/stop_service.h"
#include "infra/db.h"
#include "infra/logger.h"

using namespace urban_transport;

class StopServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        Logger::get_instance().initialize();
        db_path = "test_stops.db";
        service.initialize(db_path);
        create_test_schema();
    }

    void TearDown() override {
        Logger::get_instance().shutdown();
        std::remove(db_path.c_str());
    }

    void create_test_schema() {
        std::string sql = R"(
CREATE TABLE IF NOT EXISTS stops (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    latitude REAL NOT NULL,
    longitude REAL NOT NULL
);

CREATE TABLE IF NOT EXISTS routes (
    id INTEGER PRIMARY KEY,
    name TEXT NOT NULL,
    transport_type TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS route_stops (
    route_id INTEGER,
    stop_id INTEGER,
    sequence INTEGER NOT NULL,
    PRIMARY KEY (route_id, stop_id)
);

INSERT INTO stops (id, name, latitude, longitude) VALUES
(1, 'Parada 1', 40.7128, -74.0060),
(2, 'Parada 2', 40.7228, -74.0160),
(3, 'Parada 3', 40.7028, -73.9960);
)";

        Database db;
        if (db.connect(db_path)) {
            db.execute(sql);
            db.disconnect();
        }
    }

    StopService service;
    std::string db_path;
};

TEST_F(StopServiceTest, CreateStop) {
    Stop stop(10, "Nueva Parada", 40.7150, -74.0100);
    EXPECT_TRUE(service.create_stop(stop));

    Stop retrieved = service.get_stop(10);
    EXPECT_EQ(retrieved.id, 10);
    EXPECT_EQ(retrieved.name, "Nueva Parada");
}

TEST_F(StopServiceTest, GetAllStops) {
    auto stops = service.get_all_stops();
    EXPECT_FALSE(stops.empty());
}

TEST_F(StopServiceTest, FindNearbyStops) {
    auto nearby = service.find_nearby_stops(40.7130, -74.0065, 1.0);
    EXPECT_FALSE(nearby.empty());
}

TEST_F(StopServiceTest, UpdateStop) {
    Stop stop(1, "Parada Actualizada", 40.7200, -74.0100);
    EXPECT_TRUE(service.update_stop(stop));

    Stop updated = service.get_stop(1);
    EXPECT_EQ(updated.name, "Parada Actualizada");
}