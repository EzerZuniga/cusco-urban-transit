#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <string>
#include <vector>
#include <memory>

namespace urban_transport {

struct Stop {
    int id;
    std::string name;
    double latitude;
    double longitude;
    
    Stop(int id, const std::string& name, double lat, double lon)
        : id(id), name(name), latitude(lat), longitude(lon) {}
};

struct Route {
    int id;
    std::string name;
    std::string transport_type; // "bus", "metro", "train"
    std::vector<int> stop_ids;
    
    Route(int id, const std::string& name, const std::string& type)
        : id(id), name(name), transport_type(type) {}
};

struct Trip {
    int id;
    int route_id;
    std::string start_time;
    std::string end_time;
    std::vector<int> stop_sequence;
    
    Trip(int id, int route_id, const std::string& start, const std::string& end)
        : id(id), route_id(route_id), start_time(start), end_time(end) {}
};

class TransportSystem {
public:
    TransportSystem();
    ~TransportSystem();
    
    bool initialize(const std::string& db_path);
    void shutdown();
    
    // Gestión de paradas
    bool add_stop(const Stop& stop);
    Stop get_stop(int id) const;
    std::vector<Stop> get_all_stops() const;
    
    // Gestión de rutas
    bool add_route(const Route& route);
    Route get_route(int id) const;
    std::vector<Route> get_all_routes() const;
    
    // Gestión de viajes
    bool add_trip(const Trip& trip);
    Trip get_trip(int id) const;
    
    // Algoritmos
    std::vector<int> find_shortest_path(int start_stop, int end_stop) const;
    std::vector<Route> find_routes_through_stop(int stop_id) const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace urban_transport

#endif // TRANSPORT_H