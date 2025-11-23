#include "core/algorithms.h"
#include "core/graph.h"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <cmath>

using namespace urban_transport;

const double EARTH_RADIUS_KM = 6371.0;
const double INF = std::numeric_limits<double>::infinity();

std::vector<int> TransportAlgorithms::dijkstra_shortest_path(
    const Graph& graph, 
    int start_node, 
    int end_node) {
    
    if (!graph.has_node(start_node) || !graph.has_node(end_node)) {
        return {};
    }
    
    std::unordered_map<int, double> distances;
    std::unordered_map<int, int> previous;
    std::unordered_set<int> visited;
    
    auto nodes = graph.get_all_nodes();
    for (int node : nodes) {
        distances[node] = INF;
    }
    distances[start_node] = 0.0;
    
    // Usar priority_queue para obtener el nodo con menor distancia
    using Pair = std::pair<double, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    pq.push({0.0, start_node});
    
    while (!pq.empty()) {
        auto [current_dist, current_node] = pq.top();
        pq.pop();
        
        if (visited.count(current_node)) continue;
        visited.insert(current_node);
        
        if (current_node == end_node) {
            // Reconstruir camino
            std::vector<int> path;
            int node = end_node;
            while (node != start_node) {
                path.push_back(node);
                node = previous[node];
            }
            path.push_back(start_node);
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        for (const auto& edge : graph.get_edges(current_node)) {
            double new_dist = current_dist + edge.weight;
            if (new_dist < distances[edge.target]) {
                distances[edge.target] = new_dist;
                previous[edge.target] = current_node;
                pq.push({new_dist, edge.target});
            }
        }
    }
    
    return {}; // No se encontró camino
}

std::vector<int> TransportAlgorithms::bfs_reachable_nodes(
    const Graph& graph, 
    int start_node, 
    int max_depth) {
    
    std::vector<int> reachable;
    std::unordered_set<int> visited;
    std::queue<std::pair<int, int>> q; // {node, depth}
    
    q.push({start_node, 0});
    visited.insert(start_node);
    
    while (!q.empty()) {
        auto [current_node, depth] = q.front();
        q.pop();
        
        reachable.push_back(current_node);
        
        if (depth < max_depth) {
            for (const auto& edge : graph.get_edges(current_node)) {
                if (!visited.count(edge.target)) {
                    visited.insert(edge.target);
                    q.push({edge.target, depth + 1});
                }
            }
        }
    }
    
    return reachable;
}

std::vector<int> TransportAlgorithms::find_routes_through_stop(
    const std::unordered_map<int, std::vector<int>>& route_stops,
    int stop_id) {
    
    std::vector<int> routes;
    for (const auto& [route_id, stops] : route_stops) {
        for (int stop : stops) {
            if (stop == stop_id) {
                routes.push_back(route_id);
                break;
            }
        }
    }
    return routes;
}

double TransportAlgorithms::calculate_distance(
    double lat1, double lon1, 
    double lat2, double lon2) {
    
    // Convertir grados a radianes
    double lat1_rad = lat1 * M_PI / 180.0;
    double lon1_rad = lon1 * M_PI / 180.0;
    double lat2_rad = lat2 * M_PI / 180.0;
    double lon2_rad = lon2 * M_PI / 180.0;
    
    // Diferencia entre coordenadas
    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;
    
    // Fórmula de Haversine
    double a = std::sin(dlat/2) * std::sin(dlat/2) +
               std::cos(lat1_rad) * std::cos(lat2_rad) *
               std::sin(dlon/2) * std::sin(dlon/2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
    
    return EARTH_RADIUS_KM * c;
}