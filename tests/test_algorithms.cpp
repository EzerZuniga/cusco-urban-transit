#include <gtest/gtest.h>
#include <algorithm>
#include <unordered_map>
#include "core/algorithms.h"
#include "core/graph.h"

using namespace urban_transport;

class AlgorithmsTest : public ::testing::Test {
protected:
    void SetUp() override {
        graph.add_node(1);
        graph.add_node(2);
        graph.add_node(3);
        graph.add_node(4);

        graph.add_edge(1, 2, 1.0);
        graph.add_edge(2, 3, 2.0);
        graph.add_edge(3, 4, 1.5);
        graph.add_edge(1, 4, 5.0);
    }

    Graph graph;
};

TEST_F(AlgorithmsTest, DijkstraShortestPath) {
    auto path = TransportAlgorithms::dijkstra_shortest_path(graph, 1, 4);
    ASSERT_EQ(path.size(), 4);
    EXPECT_EQ(path[0], 1);
    EXPECT_EQ(path[1], 2);
    EXPECT_EQ(path[2], 3);
    EXPECT_EQ(path[3], 4);
}

TEST_F(AlgorithmsTest, DijkstraNoPath) {
    graph.add_node(5);
    auto path = TransportAlgorithms::dijkstra_shortest_path(graph, 1, 5);
    EXPECT_TRUE(path.empty());
}

TEST_F(AlgorithmsTest, BFSReachableNodes) {
    auto reachable = TransportAlgorithms::bfs_reachable_nodes(graph, 1, 2);
    EXPECT_GE(reachable.size(), 3);
}

TEST_F(AlgorithmsTest, CalculateDistance) {
    double ny_lat = 40.7128, ny_lon = -74.0060;
    double boston_lat = 42.3601, boston_lon = -71.0589;

    double distance = TransportAlgorithms::calculate_distance(ny_lat, ny_lon, boston_lat, boston_lon);
    EXPECT_NEAR(distance, 306, 10);
}

TEST_F(AlgorithmsTest, FindRoutesThroughStop) {
    std::unordered_map<int, std::vector<int>> route_stops = {
        {1, {101, 102, 103}},
        {2, {102, 104, 105}},
        {3, {106, 107, 108}}
    };

    auto routes = TransportAlgorithms::find_routes_through_stop(route_stops, 102);
    EXPECT_EQ(routes.size(), 2);
    EXPECT_TRUE(std::find(routes.begin(), routes.end(), 1) != routes.end());
    EXPECT_TRUE(std::find(routes.begin(), routes.end(), 2) != routes.end());
}