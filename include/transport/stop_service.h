#ifndef STOP_SERVICE_H
#define STOP_SERVICE_H

#include "transport.h"
#include <vector>

namespace urban_transport {

class StopService {
public:
    StopService();
    ~StopService();
    
    bool initialize(const std::string& db_path);
    
    // CRUD operations
    bool create_stop(const Stop& stop);
    Stop get_stop(int id) const;
    std::vector<Stop> get_all_stops() const;
    bool update_stop(const Stop& stop);
    bool delete_stop(int id);
    
    // Business logic
    std::vector<Stop> find_nearby_stops(double latitude, double longitude, double radius_km) const;
    std::vector<Route> get_routes_through_stop(int stop_id) const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace urban_transport

#endif // STOP_SERVICE_H