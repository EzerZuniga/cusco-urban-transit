#include "transport/route_service.h"
#include "infra/db.h"
#include "infra/logger.h"
#include <memory>

using namespace urban_transport;

class RouteService::Impl {
public:
    bool initialize(const std::string& db_path) {
        bool ok = db_.connect(db_path);
        if (!ok) Logger::get_instance().error("RouteService: failed to connect to DB: " + db_path);
        return ok;
    }

    bool create_route(const Route& route) {
        const std::string sql = "INSERT INTO routes (id, name, transport_type) VALUES (?, ?, ?)";
        const std::vector<std::string> params = {
            std::to_string(route.id),
            route.name,
            route.transport_type
        };

        const bool result = db_.execute_with_params(sql, params);
        if (result) Logger::get_instance().info("Route created: " + route.name);
        return result;
    }

    Route get_route(int id) const {
        const std::string sql = "SELECT id, name, transport_type FROM routes WHERE id = ?";
        const std::vector<std::string> params = {std::to_string(id)};

        Route route(0, "", "");
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            route.id = std::stoi(row[0]);
            route.name = row[1];
            route.transport_type = row[2];
            return false;
        });

        route.stop_ids = get_route_stops(id);
        return route;
    }

    std::vector<Route> get_all_routes() const {
        std::vector<Route> routes;
        const std::string sql = "SELECT id, name, transport_type FROM routes ORDER BY id";

        db_.query(sql, [&](const std::vector<std::string>& row) {
            Route route(std::stoi(row[0]), row[1], row[2]);
            route.stop_ids = get_route_stops(route.id);
            routes.push_back(route);
            return true;
        });

        return routes;
    }

    bool update_route(const Route& route) {
        const std::string sql = "UPDATE routes SET name = ?, transport_type = ? WHERE id = ?";
        const std::vector<std::string> params = {
            route.name,
            route.transport_type,
            std::to_string(route.id)
        };

        return db_.execute_with_params(sql, params);
    }

    bool delete_route(int id) {
        const std::string sql1 = "DELETE FROM route_stops WHERE route_id = ?";
        const std::vector<std::string> params1 = {std::to_string(id)};
        db_.execute_with_params(sql1, params1);

        const std::string sql2 = "DELETE FROM routes WHERE id = ?";
        const std::vector<std::string> params2 = {std::to_string(id)};

        return db_.execute_with_params(sql2, params2);
    }

    std::vector<Route> find_routes_by_type(const std::string& transport_type) const {
        std::vector<Route> routes;
        const std::string sql = "SELECT id, name, transport_type FROM routes WHERE transport_type = ? ORDER BY id";
        const std::vector<std::string> params = {transport_type};

        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            Route route(std::stoi(row[0]), row[1], row[2]);
            route.stop_ids = get_route_stops(route.id);
            routes.push_back(route);
            return true;
        });

        return routes;
    }

    std::vector<int> get_route_stops(int route_id) const {
        std::vector<int> stops;
        const std::string sql = "SELECT stop_id FROM route_stops WHERE route_id = ? ORDER BY sequence";
        const std::vector<std::string> params = {std::to_string(route_id)};

        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            stops.push_back(std::stoi(row[0]));
            return true;
        });

        return stops;
    }

    bool add_stop_to_route(int route_id, int stop_id) {
        const auto stops = get_route_stops(route_id);
        const int sequence = static_cast<int>(stops.size()) + 1;

        const std::string sql = "INSERT INTO route_stops (route_id, stop_id, sequence) VALUES (?, ?, ?)";
        const std::vector<std::string> params = {
            std::to_string(route_id),
            std::to_string(stop_id),
            std::to_string(sequence)
        };

        return db_.execute_with_params(sql, params);
    }

    bool remove_stop_from_route(int route_id, int stop_id) {
        const std::string sql = "DELETE FROM route_stops WHERE route_id = ? AND stop_id = ?";
        const std::vector<std::string> params = {
            std::to_string(route_id),
            std::to_string(stop_id)
        };

        return db_.execute_with_params(sql, params);
    }

private:
    Database db_;
};

RouteService::RouteService() : pimpl(std::make_unique<Impl>()) {}
RouteService::~RouteService() = default;

bool RouteService::initialize(const std::string& db_path) {
    return pimpl->initialize(db_path);
}

bool RouteService::create_route(const Route& route) {
    return pimpl->create_route(route);
}

Route RouteService::get_route(int id) const {
    return pimpl->get_route(id);
}

std::vector<Route> RouteService::get_all_routes() const {
    return pimpl->get_all_routes();
}

bool RouteService::update_route(const Route& route) {
    return pimpl->update_route(route);
}

bool RouteService::delete_route(int id) {
    return pimpl->delete_route(id);
}

std::vector<Route> RouteService::find_routes_by_type(const std::string& transport_type) const {
    return pimpl->find_routes_by_type(transport_type);
}

std::vector<int> RouteService::get_route_stops(int route_id) const {
    return pimpl->get_route_stops(route_id);
}

bool RouteService::add_stop_to_route(int route_id, int stop_id) {
    return pimpl->add_stop_to_route(route_id, stop_id);
}

bool RouteService::remove_stop_from_route(int route_id, int stop_id) {
    return pimpl->remove_stop_from_route(route_id, stop_id);
}