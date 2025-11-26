#include "transport/transport.h"
#include "infra/db.h"
#include "infra/logger.h"
#include "core/algorithms.h"
#include <memory>
#include <unordered_map>

using namespace urban_transport;

class TransportSystem::Impl {
public:
    bool initialize(const std::string& db_path) {
        if (!db_.connect(db_path)) {
            Logger::get_instance().error("Failed to connect to database");
            return false;
        }

        initialize_graph();

        Logger::get_instance().info("Transport system initialized");
        return true;
    }
    
    void shutdown() {
        db_.disconnect();
        Logger::get_instance().info("Transport system shutdown");
    }
    
    bool add_stop(const Stop& stop) {
        std::string sql = "INSERT INTO stops (id, name, latitude, longitude) VALUES (?, ?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(stop.id),
            stop.name,
            std::to_string(stop.latitude),
            std::to_string(stop.longitude)
        };
        
        bool result = db_.execute_with_params(sql, params);
        if (result) {
            graph_.add_node(stop.id);
            Logger::get_instance().info("Stop added: " + stop.name);
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
            return false; // Solo primera fila
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
    
    bool add_route(const Route& route) {
        std::string sql = "INSERT INTO routes (id, name, transport_type) VALUES (?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(route.id),
            route.name,
            route.transport_type
        };
        
        bool result = db_.execute_with_params(sql, params);
        if (result) {
            for (int stop_id : route.stop_ids) add_stop_to_route(route.id, stop_id);
            Logger::get_instance().info("Route added: " + route.name);
        }
        return result;
    }
    
    Route get_route(int id) const {
        std::string sql = "SELECT id, name, transport_type FROM routes WHERE id = ?";
        std::vector<std::string> params = {std::to_string(id)};
        
        Route route(0, "", "");
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            route.id = std::stoi(row[0]);
            route.name = row[1];
            route.transport_type = row[2];
            return false;
        });
        
        // Obtener paradas de la ruta
        route.stop_ids = get_route_stops(id);
        
        return route;
    }
    
    std::vector<Route> get_all_routes() const {
        std::vector<Route> routes;
        std::string sql = "SELECT id, name, transport_type FROM routes ORDER BY id";
        
        db_.query(sql, [&](const std::vector<std::string>& row) {
            Route route(std::stoi(row[0]), row[1], row[2]);
            route.stop_ids = get_route_stops(route.id);
            routes.push_back(route);
            return true;
        });
        
        return routes;
    }
    
    std::vector<int> find_shortest_path(int start_stop, int end_stop) const {
        return TransportAlgorithms::dijkstra_shortest_path(graph_, start_stop, end_stop);
    }
    
    std::vector<Route> find_routes_through_stop(int stop_id) const {
        std::vector<Route> routes;
        std::string sql = 
            "SELECT r.id, r.name, r.transport_type "
            "FROM routes r "
            "JOIN route_stops rs ON r.id = rs.route_id "
            "WHERE rs.stop_id = ?";
        
        std::vector<std::string> params = {std::to_string(stop_id)};
        
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            Route route(std::stoi(row[0]), row[1], row[2]);
            route.stop_ids = get_route_stops(route.id);
            routes.push_back(route);
            return true;
        });
        
        return routes;
    }

private:
    Database db_;
    Graph graph_;
    std::unordered_map<int, std::vector<int>> route_stops_;
    
    void initialize_graph() {
        auto stops = get_all_stops();
        for (const auto& stop : stops) graph_.add_node(stop.id);

        auto routes = get_all_routes();
        for (const auto& route : routes) {
            if (route.stop_ids.size() < 2) continue;
            for (size_t i = 0; i + 1 < route.stop_ids.size(); ++i) {
                int from = route.stop_ids[i];
                int to = route.stop_ids[i + 1];

                auto stop1 = get_stop(from);
                auto stop2 = get_stop(to);
                double distance = TransportAlgorithms::calculate_distance(
                    stop1.latitude, stop1.longitude,
                    stop2.latitude, stop2.longitude);

                graph_.add_edge(from, to, distance);
                graph_.add_edge(to, from, distance);
            }
        }
    }
    
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
    
    bool add_stop_to_route(int route_id, int stop_id) {
        std::string sql = "INSERT INTO route_stops (route_id, stop_id, sequence) VALUES (?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(route_id),
            std::to_string(stop_id),
            std::to_string(get_route_stops(route_id).size() + 1)
        };
        
        return db_.execute_with_params(sql, params);
    }
};

// Implementación de TransportSystem
TransportSystem::TransportSystem() : pimpl(std::make_unique<Impl>()) {}
TransportSystem::~TransportSystem() = default;

bool TransportSystem::initialize(const std::string& db_path) {
    return pimpl->initialize(db_path);
}

void TransportSystem::shutdown() {
    pimpl->shutdown();
}

bool TransportSystem::add_stop(const Stop& stop) {
    return pimpl->add_stop(stop);
}

Stop TransportSystem::get_stop(int id) const {
    return pimpl->get_stop(id);
}

std::vector<Stop> TransportSystem::get_all_stops() const {
    return pimpl->get_all_stops();
}

bool TransportSystem::add_route(const Route& route) {
    return pimpl->add_route(route);
}

Route TransportSystem::get_route(int id) const {
    return pimpl->get_route(id);
}

std::vector<Route> TransportSystem::get_all_routes() const {
    return pimpl->get_all_routes();
}

bool TransportSystem::add_trip(const Trip& trip) {
    // Implementación básica - se puede expandir
    return true;
}

Trip TransportSystem::get_trip(int id) const {
    return Trip(0, 0, "", "");
}

std::vector<int> TransportSystem::find_shortest_path(int start_stop, int end_stop) const {
    return pimpl->find_shortest_path(start_stop, end_stop);
}

std::vector<Route> TransportSystem::find_routes_through_stop(int stop_id) const {
    return pimpl->find_routes_through_stop(stop_id);
}