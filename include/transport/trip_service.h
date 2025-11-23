#ifndef TRIP_SERVICE_H
#define TRIP_SERVICE_H

#include "transport.h"
#include <vector>

namespace urban_transport {

class TripService {
public:
    TripService();
    ~TripService();
    
    bool initialize(const std::string& db_path);
    
    // CRUD operations
    bool create_trip(const Trip& trip);
    Trip get_trip(int id) const;
    std::vector<Trip> get_all_trips() const;
    bool update_trip(const Trip& trip);
    bool delete_trip(int id);
    
    // Business logic
    std::vector<Trip> find_trips_by_route(int route_id) const;
    std::vector<Trip> find_trips_by_time_range(const std::string& start_time, 
                                              const std::string& end_time) const;
    bool add_stop_to_trip(int trip_id, int stop_id, int sequence);
    std::vector<int> get_trip_stops(int trip_id) const;

private:
    class Impl;
    std::unique_ptr<Impl> pimpl;
};

} // namespace urban_transport

#endif // TRIP_SERVICE_H