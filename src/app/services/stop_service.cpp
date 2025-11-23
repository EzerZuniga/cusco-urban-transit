#include "transport/stop_service.h"
#include "infra/db.h"
#include "infra/logger.h"
#include "core/algorithms.h"
#include <memory>
#include <cmath>

using namespace urban_transport;

class StopService::Impl {
public:
    bool initialize(const std::string& db_path) {
        return db_.connect(db_path);
    }
    
    bool create_stop(const Stop& stop) {
        std::string sql = "INSERT INTO stops (id, name, latitude, longitude) VALUES (?, ?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(stop.id),
            stop.name,
            std::to_string(stop.latitude),
            std::to_string(stop.longitude)
        };
        
        bool result = db_.execute_with_params(sql, params);
        if (result) {
            Logger::get_instance().info("Parada creada: " + stop.name);
        }
        return result;
    }
    
    Stop get_stop(int id) const {
        std::string sql = "SELECT id, name, latitude, longitude FROM stops WHERE id = ?";
        std::vector<std::string> params = {std::to_string(id)};
        
        Stop stop(0, "", 0, 0);
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            stop.id = std::stoi(row[0]);
            stop.name = row[1];
            stop.latitude = std::stod(row[2]);
            stop.longitude = std::stod(row[3]);
            return false;
        });
        
        return stop;
    }
    
    std::vector<Stop> get_all_stops() const {
        std::vector<Stop> stops;
        std::string sql = "SELECT id, name, latitude, longitude FROM stops ORDER BY id";
        
        db_.query(sql, [&](const std::vector<std::string>& row) {
            stops.emplace_back(
                std::stoi(row[0]),
                row[1],
                std::stod(row[2]),
                std::stod(row[3])
            );
            return true;
        });
        
        return stops;
    }
    
    bool update_stop(const Stop& stop) {
        std::string sql = "UPDATE stops SET name = ?, latitude = ?, longitude = ? WHERE id = ?";
        std::vector<std::string> params = {
            stop.name,
            std::to_string(stop.latitude),
            std::to_string(stop.longitude),
            std::to_string(stop.id)
        };
        
        return db_.execute_with_params(sql, params);
    }
    
    bool delete_stop(int id) {
        std::string sql = "DELETE FROM stops WHERE id = ?";
        std::vector<std::string> params = {std::to_string(id)};
        
        return db_.execute_with_params(sql, params);
    }
    
    std::vector<Stop> find_nearby_stops(double latitude, double longitude, double radius_km) const {
        std::vector<Stop> nearby_stops;
        auto all_stops = get_all_stops();
        
        for (const auto& stop : all_stops) {
            double distance = TransportAlgorithms::calculate_distance(
                latitude, longitude,
                stop.latitude, stop.longitude
            );
            
            if (distance <= radius_km) {
                nearby_stops.push_back(stop);
            }
        }
        
        return nearby_stops;
    }
    
    std::vector<Route> get_routes_through_stop(int stop_id) const {
        std::vector<Route> routes;
        std::string sql = 
            "SELECT r.id, r.name, r.transport_type "
            "FROM routes r "
            "JOIN route_stops rs ON r.id = rs.route_id "
            "WHERE rs.stop_id = ?";
        
        std::vector<std::string> params = {std::to_string(stop_id)};
        
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            Route route(std::stoi(row[0]), row[1], row[2]);
            // Obtener paradas de la ruta
            route.stop_ids = get_route_stops(route.id);
            routes.push_back(route);
            return true;
        });
        
        return routes;
    }

private:
    Database db_;
    
    std::vector<int> get_route_stops(int route_id) const {
        std::vector<int> stops;
        std::string sql = "SELECT stop_id FROM route_stops WHERE route_id = ? ORDER BY sequence";
        std::vector<std::string> params = {std::to_string(route_id)};
        
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            stops.push_back(std::stoi(row[0]));
            return true;
        });
        
        return stops;
    }
};

// Implementación de StopService
StopService::StopService() : pimpl(std::make_unique<Impl>()) {}
StopService::~StopService() = default;

bool StopService::initialize(const std::string& db_path) {
    return pimpl->initialize(db_path);
}

bool StopService::create_stop(const Stop& stop) {
    return pimpl->create_stop(stop);
}

Stop StopService::get_stop(int id) const {
    return pimpl->get_stop(id);
}

std::vector<Stop> StopService::get_all_stops() const {
    return pimpl->get_all_stops();
}

bool StopService::update_stop(const Stop& stop) {
    return pimpl->update_stop(stop);
}

bool StopService::delete_stop(int id) {
    return pimpl->delete_stop(id);
}

std::vector<Stop> StopService::find_nearby_stops(double latitude, double longitude, double radius_km) const {
    return pimpl->find_nearby_stops(latitude, longitude, radius_km);
}

std::vector<Route> StopService::get_routes_through_stop(int stop_id) const {
    return pimpl->get_routes_through_stop(stop_id);
}