#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "graph.h"
#include <vector>
#include <unordered_map>

namespace urban_transport {

class TransportAlgorithms {
public:
    // Dijkstra para camino más corto
    static std::vector<int> dijkstra_shortest_path(
        const Graph& graph, 
        int start_node, 
        int end_node);
    
    // BFS para exploración
    static std::vector<int> bfs_reachable_nodes(
        const Graph& graph, 
        int start_node, 
        int max_depth);
    
    // Algoritmo para rutas que pasan por una parada
    static std::vector<int> find_routes_through_stop(
        const std::unordered_map<int, std::vector<int>>& route_stops,
        int stop_id);
    
    // Cálculo de distancia entre coordenadas (Haversine)
    static double calculate_distance(
        double lat1, double lon1, 
        double lat2, double lon2);
};

} // namespace urban_transport

#endif // ALGORITHMS_H