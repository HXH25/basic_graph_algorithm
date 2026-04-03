#pragma once
#include "matrix.h"
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

/** Kruskal 专用：无向邻接中「i 与 j 之间无边」须为该值；边权可为 0 或负数，但不能等于此值。仅使用 i<j 与矩阵上三角，不考虑自环。 */
inline constexpr int kruskal_no_edge() noexcept {
  return std::numeric_limits<int>::max();
}

adjacency_matrix warshall(adjacency_matrix graph);
adjacency_matrix warshall(adjacency_matrix graph, int i);

void hamilton_one_solution(adjacency_matrix &graph, std::vector<bool> &visited,
                           std::vector<int> curr_route, int curr_position);
std::vector<std::vector<int>> hamilton(adjacency_matrix graph);

void branch_and_bound_solution();
int branch_and_bound();

std::vector<int> dijkstra(int start, std::vector<std::vector<int>> graph);

// 无向带权图 Kruskal：对 i<j，若 und_adj[i][j]==kruskal_no_edge() 则无边；否则加入边权 und_adj[i][j]（可为 0）。
// 对角线/自环不纳入；若不连通则 connected == false。
struct KruskalMstResult {
  bool connected{};
  long long total_weight{};
  std::vector<std::tuple<int, int, int>> mst_edges; // (u, v, w)，u < v
};
KruskalMstResult kruskal_mst(const std::vector<std::vector<int>> &und_adj);

// Huffman：叶权非负，至少 2 个正权。返回可打印的建树与编码说明。
std::string huffman_complete_binary_report(const std::vector<int> &leaf_weights);

// Ford（Bellman-Ford）：graph[i][j]==0 表示 i→j 无边，否则为有向边权（可为负）。
// 若有从起点可达的负环则 has_negative_cycle=true。
std::vector<long long> ford_bellman(int start,
                                      const std::vector<std::vector<int>> &graph,
                                      bool &has_negative_cycle);

// 旅行商（无权图上的 Hamilton 回路）：基于关联矩阵拓扑，DFS 求一条回路 v0→…→v0
std::vector<int> tsp_DFS(const incidence_matrix &input);