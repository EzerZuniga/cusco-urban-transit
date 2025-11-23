#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <utility>

namespace urban_transport {

struct Edge {
    int target;
    double weight;
    
    Edge(int t, double w) : target(t), weight(w) {}
};

class Graph {
public:
    Graph() = default;
    
    void add_node(int node_id);
    void add_edge(int from, int to, double weight);
    void remove_edge(int from, int to);
    
    const std::vector<Edge>& get_edges(int node_id) const;
    bool has_node(int node_id) const;
    size_t node_count() const;
    
    std::vector<int> get_all_nodes() const;

private:
    std::unordered_map<int, std::vector<Edge>> adjacency_list;
};

} // namespace urban_transport

#endif // GRAPH_H