#pragma once
#include "matrix.h"
#include <iostream>
#include <vector>


adjacency_matrix warshall(adjacency_matrix graph);
adjacency_matrix warshall(adjacency_matrix graph, int i);

void hamilton_one_solution(adjacency_matrix &graph, std::vector<bool> &visited,
                           std::vector<int> curr_route, int curr_position);
std::vector<std::vector<int>> hamilton(adjacency_matrix graph);

void branch_and_bound_solution();
int branch_and_bound();

std::vector<int> dijkstra(int start, std::vector<std::vector<int>> graph);