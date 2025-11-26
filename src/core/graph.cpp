#include "core/graph.h"
#include <algorithm>

using namespace urban_transport;

void Graph::add_node(int node_id) {
    if (adjacency_list.find(node_id) == adjacency_list.end()) {
        adjacency_list.emplace(node_id, std::vector<Edge>());
    }
}

void Graph::add_edge(int from, int to, double weight) {
    add_node(from);
    add_node(to);
    adjacency_list[from].push_back(Edge(to, weight));
}

void Graph::remove_edge(int from, int to) {
    auto it = adjacency_list.find(from);
    if (it == adjacency_list.end()) return;
    auto &edges = it->second;
    edges.erase(std::remove_if(edges.begin(), edges.end(), [&](const Edge &e) { return e.target == to; }), edges.end());
}

const std::vector<Edge>& Graph::get_edges(int node_id) const {
    static const std::vector<Edge> empty;
    auto it = adjacency_list.find(node_id);
    if (it == adjacency_list.end()) return empty;
    return it->second;
}

bool Graph::has_node(int node_id) const {
    return adjacency_list.find(node_id) != adjacency_list.end();
}

size_t Graph::node_count() const {
    return adjacency_list.size();
}

std::vector<int> Graph::get_all_nodes() const {
    std::vector<int> nodes;
    nodes.reserve(adjacency_list.size());
    for (const auto &p : adjacency_list) {
        nodes.push_back(p.first);
    }
    return nodes;
}
