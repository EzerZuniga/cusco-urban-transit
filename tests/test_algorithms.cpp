#include <gtest/gtest.h>
#include "core/algorithms.h"
#include "core/graph.h"

using namespace urban_transport;

class AlgorithmsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Crear grafo de prueba
        graph.add_node(1);
        graph.add_node(2);
        graph.add_node(3);
        graph.add_node(4);
        
        graph.add_edge(1, 2, 1.0);
        graph.add_edge(2, 3, 2.0);
        graph.add_edge(3, 4, 1.5);
        graph.add_edge(1, 4, 5.0); // Camino directo más largo
    }
    
    Graph graph;
};

TEST_F(AlgorithmsTest, DijkstraShortestPath) {
    auto path = TransportAlgorithms::dijkstra_shortest_path(graph, 1, 4);
    
    // El camino más corto debería ser 1->2->3->4 (peso 4.5) en lugar de 1->4 (peso 5.0)
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], 1);
    EXPECT_EQ(path[1], 2);
    EXPECT_EQ(path[2], 3);
    EXPECT_EQ(path[3], 4);
}

TEST_F(AlgorithmsTest, DijkstraNoPath) {
    graph.add_node(5); // Nodo aislado
    
    auto path = TransportAlgorithms::dijkstra_shortest_path(graph, 1, 5);
    EXPECT_TRUE(path.empty());
}

TEST_F(AlgorithmsTest, BFSReachableNodes) {
    auto reachable = TransportAlgorithms::bfs_reachable_nodes(graph, 1, 2);
    
    // Desde el nodo 1 con profundidad 2 deberíamos alcanzar nodos 1,2,3,4
    EXPECT_GE(reachable.size(), 3);
}

TEST_F(AlgorithmsTest, CalculateDistance) {
    // Coordenadas de Nueva York
    double ny_lat = 40.7128, ny_lon = -74.0060;
    // Coordenadas de Boston (aproximadamente)
    double boston_lat = 42.3601, boston_lon = -71.0589;
    
    double distance = TransportAlgorithms::calculate_distance(ny_lat, ny_lon, boston_lat, boston_lon);
    
    // La distancia NY-Boston es aproximadamente 306km
    EXPECT_NEAR(distance, 306, 10); // ±10km de tolerancia
}

TEST_F(AlgorithmsTest, FindRoutesThroughStop) {
    std::unordered_map<int, std::vector<int>> route_stops = {
        {1, {101, 102, 103}},
        {2, {102, 104, 105}},
        {3, {106, 107, 108}}
    };
    
    auto routes = TransportAlgorithms::find_routes_through_stop(route_stops, 102);
    
    EXPECT_EQ(routes.size(), 2); // Rutas 1 y 2 pasan por la parada 102
    EXPECT_TRUE(std::find(routes.begin(), routes.end(), 1) != routes.end());
    EXPECT_TRUE(std::find(routes.begin(), routes.end(), 2) != routes.end());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}