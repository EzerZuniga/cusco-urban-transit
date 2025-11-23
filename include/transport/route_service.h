#ifndef ROUTE_SERVICE_H
#define ROUTE_SERVICE_H

#include "transport.h"
#include <vector>

namespace urban_transport {

class RouteService {
public:
    RouteService();
    ~RouteService();
    
    bool initialize(const std::string& db_path);
    
    // CRUD operations
    bool create_route(const Route& route);
    Route get_route(int id) const;
    std::vector<Route> get_all_routes() const;
    bool update_route(const Route& route);
    bool delete_route(int id);
    
    // Business logic
    std::vector<Route> find_routes_by_type(const std::string& transport_type) const;
    std::vector<int> get_route_stops(int route_id) const;
    bool add_stop_to_route(int route_id, int stop_id);
    bool remove_stop_from_route(int route_id, int stop_id);

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace urban_transport

#endif // ROUTE_SERVICE_H