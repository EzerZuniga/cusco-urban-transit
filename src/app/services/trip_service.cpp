#include "transport/trip_service.h"
#include "infra/db.h"
#include "infra/logger.h"
#include <memory>

using namespace urban_transport;

class TripService::Impl {
public:
    bool initialize(const std::string& db_path) {
        return db_.connect(db_path);
    }

    bool create_trip(const Trip& trip) {
        std::string sql = "INSERT INTO trips (id, route_id, start_time, end_time) VALUES (?, ?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(trip.id),
            std::to_string(trip.route_id),
            trip.start_time,
            trip.end_time
        };

        bool result = db_.execute_with_params(sql, params);
        if (result) {
            Logger::get_instance().info("Trip created: id=" + std::to_string(trip.id));
        }
        return result;
    }

    Trip get_trip(int id) const {
        std::string sql = "SELECT id, route_id, start_time, end_time FROM trips WHERE id = ?";
        std::vector<std::string> params = {std::to_string(id)};

        Trip trip(0, 0, "", "");
        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            trip.id = std::stoi(row[0]);
            trip.route_id = std::stoi(row[1]);
            trip.start_time = row[2];
            trip.end_time = row[3];
            return false; // detener después de la primera fila
        });

        trip.stop_sequence = get_trip_stops(id);
        return trip;
    }

    std::vector<Trip> get_all_trips() const {
        std::vector<Trip> trips;
        std::string sql = "SELECT id, route_id, start_time, end_time FROM trips ORDER BY id";

        db_.query(sql, [&](const std::vector<std::string>& row) {
            Trip trip(std::stoi(row[0]), std::stoi(row[1]), row[2], row[3]);
            trip.stop_sequence = get_trip_stops(trip.id);
            trips.push_back(trip);
            return true;
        });

        return trips;
    }

    bool update_trip(const Trip& trip) {
        std::string sql = "UPDATE trips SET route_id = ?, start_time = ?, end_time = ? WHERE id = ?";
        std::vector<std::string> params = {
            std::to_string(trip.route_id),
            trip.start_time,
            trip.end_time,
            std::to_string(trip.id)
        };

        return db_.execute_with_params(sql, params);
    }

    bool delete_trip(int id) {
        // eliminar relaciones con paradas
        std::string sql1 = "DELETE FROM trip_stops WHERE trip_id = ?";
        std::vector<std::string> params1 = {std::to_string(id)};
        db_.execute_with_params(sql1, params1);

        std::string sql2 = "DELETE FROM trips WHERE id = ?";
        std::vector<std::string> params2 = {std::to_string(id)};
        return db_.execute_with_params(sql2, params2);
    }

    std::vector<Trip> find_trips_by_route(int route_id) const {
        std::vector<Trip> trips;
        std::string sql = "SELECT id, route_id, start_time, end_time FROM trips WHERE route_id = ? ORDER BY id";
        std::vector<std::string> params = {std::to_string(route_id)};

        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            Trip trip(std::stoi(row[0]), std::stoi(row[1]), row[2], row[3]);
            trip.stop_sequence = get_trip_stops(trip.id);
            trips.push_back(trip);
            return true;
        });

        return trips;
    }

    std::vector<Trip> find_trips_by_time_range(const std::string& start_time, const std::string& end_time) const {
        std::vector<Trip> trips;
        std::string sql = "SELECT id, route_id, start_time, end_time FROM trips WHERE start_time >= ? AND end_time <= ? ORDER BY start_time";
        std::vector<std::string> params = {start_time, end_time};

        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            Trip trip(std::stoi(row[0]), std::stoi(row[1]), row[2], row[3]);
            trip.stop_sequence = get_trip_stops(trip.id);
            trips.push_back(trip);
            return true;
        });

        return trips;
    }

    bool add_stop_to_trip(int trip_id, int stop_id, int sequence) {
        // if sequence <= 0, append to end
        if (sequence <= 0) {
            auto stops = get_trip_stops(trip_id);
            sequence = static_cast<int>(stops.size()) + 1;
        }

        std::string sql = "INSERT INTO trip_stops (trip_id, stop_id, arrival_time, sequence) VALUES (?, ?, ?, ?)";
        std::vector<std::string> params = {
            std::to_string(trip_id),
            std::to_string(stop_id),
            std::string(""), // arrival_time unknown
            std::to_string(sequence)
        };

        return db_.execute_with_params(sql, params);
    }

    std::vector<int> get_trip_stops(int trip_id) const {
        std::vector<int> stops;
        std::string sql = "SELECT stop_id FROM trip_stops WHERE trip_id = ? ORDER BY sequence";
        std::vector<std::string> params = {std::to_string(trip_id)};

        db_.query_with_params(sql, params, [&](const std::vector<std::string>& row) {
            stops.push_back(std::stoi(row[0]));
            return true;
        });

        return stops;
    }

private:
    Database db_;
};

// Implementación de TripService
TripService::TripService() : pimpl(std::make_unique<Impl>()) {}
TripService::~TripService() = default;

bool TripService::initialize(const std::string& db_path) {
    return pimpl->initialize(db_path);
}

bool TripService::create_trip(const Trip& trip) {
    return pimpl->create_trip(trip);
}

Trip TripService::get_trip(int id) const {
    return pimpl->get_trip(id);
}

std::vector<Trip> TripService::get_all_trips() const {
    return pimpl->get_all_trips();
}

bool TripService::update_trip(const Trip& trip) {
    return pimpl->update_trip(trip);
}

bool TripService::delete_trip(int id) {
    return pimpl->delete_trip(id);
}

std::vector<Trip> TripService::find_trips_by_route(int route_id) const {
    return pimpl->find_trips_by_route(route_id);
}

std::vector<Trip> TripService::find_trips_by_time_range(const std::string& start_time, const std::string& end_time) const {
    return pimpl->find_trips_by_time_range(start_time, end_time);
}

bool TripService::add_stop_to_trip(int trip_id, int stop_id, int sequence) {
    return pimpl->add_stop_to_trip(trip_id, stop_id, sequence);
}

std::vector<int> TripService::get_trip_stops(int trip_id) const {
    return pimpl->get_trip_stops(trip_id);
}
