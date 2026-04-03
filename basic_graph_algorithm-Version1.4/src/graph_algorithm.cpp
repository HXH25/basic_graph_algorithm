#include "graph_algorithm.h"
#include "matrix.h"
#include <algorithm>
#include <climits>
#include <functional>
#include <iostream>
#include <limits>
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

adjacency_matrix
warshall(adjacency_matrix graph) // warshall算法:Pjk ← Pjk ∨ （Pji ∧ Pik)
{
  for (int i = 0; i < graph.size(); i++)
  {
    for (int j = 0; j < graph.size(); j++)
    {
      for (int k = 0; k < graph[j].size(); k++)
      {
        graph[j][k] = graph[j][k] | (graph[j][i] & graph[i][k]);
      }
    }
  }

  return graph;
}

adjacency_matrix warshall(adjacency_matrix graph, int i) // 重载warshall,支持一步一步模拟
{
  for (int j = 0; j < graph.size(); j++)
  {
    for (int k = 0; k < graph[j].size(); k++)
    {
      graph[j][k] = graph[j][k] | (graph[j][i] & graph[i][k]);
    }
  }

  return graph;
}

std::vector<std::vector<int>> result = {};
void hamilton_one_solution(adjacency_matrix &graph, std::vector<bool> &visited,
                           std::vector<int> curr_route, int curr_position)
{
  if (curr_route.size() == graph.size())
  {
    result.push_back(curr_route);
    return;
  }

  for (int i = 0; i < graph[curr_position].size(); i++)
  {
    if (graph[curr_position][i] != 1)
      continue;

    if (!visited[i])
    {
      std::vector<bool> new_visited = visited;
      new_visited[i] = true;
      std::vector<int> new_curr_route = curr_route;
      new_curr_route.push_back(i);
      int new_curr_position = i;
      hamilton_one_solution(graph, new_visited, new_curr_route,
                            new_curr_position);
    }
  }

  return;
}
std::vector<std::vector<int>> hamilton(adjacency_matrix graph)
{
  std::vector<bool> visited(graph.size(), false);
  int curr_position = 0;
  std::vector<int> curr_route = {};
  hamilton_one_solution(graph, visited, curr_route, curr_position);

  std::vector<std::vector<int>> return_result = result;
  result = {};
  return return_result;
}

std::vector<int> dijkstra(int start, std::vector<std::vector<int>> graph)
{
  int n = graph.size();
  std::vector<int> weights(n, INT_MAX);
  std::vector<bool> visited(n, false);
  int curr = start;

  visited[curr] = true;
  weights[curr] = 0;

  // 循环 n-1 次，因为起始节点已经处理过了
  for (int count = 0; count < n - 1; count++)
  {
    // 更新当前节点的所有邻居的距离
    for (int i = 0; i < n; i++)
    {
      // 检查是否有边从 curr 到 i，且边的权重为正，且当前节点已访问，且通过 curr
      // 到 i 的距离更短
      if (!visited[i] && graph[curr][i] > 0 && weights[curr] != INT_MAX &&
          weights[curr] + graph[curr][i] < weights[i])
      {
        weights[i] = weights[curr] + graph[curr][i];
      }
    }

    // 选择未访问的节点中距离最小的作为新的当前节点
    int min_weight = INT_MAX;
    int new_curr = -1;
    for (int i = 0; i < n; i++)
    {
      if (!visited[i] && weights[i] < min_weight)
      {
        min_weight = weights[i];
        new_curr = i;
      }
    }

    // 如果没有找到新的节点，说明图不连通，退出循环
    if (new_curr == -1)
      break;

    // 更新当前节点并标记为已访问
    curr = new_curr;
    visited[curr] = true;
  }

  return weights;
}

namespace {

struct Dsu {
  std::vector<int> p;
  explicit Dsu(int n) : p(n) { std::iota(p.begin(), p.end(), 0); }
  int find(int x) { return p[x] == x ? x : (p[x] = find(p[x])); }
  bool unite(int a, int b) {
    a = find(a);
    b = find(b);
    if (a == b)
      return false;
    p[a] = b;
    return true;
  }
};

} // namespace

KruskalMstResult kruskal_mst(const std::vector<std::vector<int>> &und_adj) {
  KruskalMstResult res;
  const int n = static_cast<int>(und_adj.size());
  if (n == 0)
    return res;
  if (n == 1) {
    res.connected = true;
    res.total_weight = 0;
    return res;
  }

  struct Edge {
    int u, v, w;
  };
  std::vector<Edge> edges;
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      const int w = und_adj[i][j];
      if (w == kruskal_no_edge())
        continue;
      edges.push_back({i, j, w});
    }
  }
  std::sort(edges.begin(), edges.end(),
            [](const Edge &a, const Edge &b) { return a.w < b.w; });

  Dsu dsu(n);
  int taken = 0;
  for (const auto &e : edges) {
    if (dsu.unite(e.u, e.v)) {
      res.mst_edges.push_back({e.u, e.v, e.w});
      res.total_weight += e.w;
      ++taken;
      if (taken == n - 1)
        break;
    }
  }

  res.connected = (taken == n - 1);
  if (!res.connected) {
    res.mst_edges.clear();
    res.total_weight = 0;
  }
  return res;
}

std::string huffman_complete_binary_report(const std::vector<int> &leaf_weights) {
  std::ostringstream out;
  std::vector<int> w = leaf_weights;
  if (w.size() < 2) {
    out << "Huffman：至少需要 2 个叶结点权值。\n";
    return out.str();
  }
  int pos = 0;
  for (int x : w) {
    if (x < 0) {
      out << "Huffman：叶权必须为非负整数。\n";
      return out.str();
    }
    pos += x;
  }
  if (pos == 0) {
    out << "Huffman：权值和为 0，无法建树。\n";
    return out.str();
  }

  // 补足标准 Huffman：若当前叶数不足以反复两两合并为一棵满二叉树，可补 0 权虚叶（完全二叉树常用技巧）
  int m = static_cast<int>(w.size());
  while (m > 1 && (m % 2 != 0)) {
    w.push_back(0);
    ++m;
  }

  struct Node {
    long long weight;
    int left;  // -1 叶
    int right; // -1 叶
    int leaf_index;
  };
  struct Item {
    long long w;
    int idx;
    bool operator>(const Item &o) const {
      if (w != o.w)
        return w > o.w;
      return idx > o.idx;
    }
  };

  std::vector<Node> nodes;
  std::priority_queue<Item, std::vector<Item>, std::greater<Item>> pq;
  for (int i = 0; i < static_cast<int>(w.size()); ++i) {
    nodes.push_back({static_cast<long long>(w[i]), -1, -1, i});
    pq.push({static_cast<long long>(w[i]), static_cast<int>(nodes.size() - 1)});
  }

  out << "Huffman（合并次序）：\n";
  int step = 1;
  while (pq.size() >= 2) {
    Item a = pq.top();
    pq.pop();
    Item b = pq.top();
    pq.pop();
    Node na = nodes[a.idx];
    Node nb = nodes[b.idx];
    nodes.push_back({na.weight + nb.weight, a.idx, b.idx, -1});
    int pidx = static_cast<int>(nodes.size() - 1);
    out << "  步骤 " << step++ << "：合并权 " << na.weight << " 与 " << nb.weight
        << " -> 新结点权 " << nodes[pidx].weight << "\n";
    pq.push({nodes[pidx].weight, pidx});
  }

  const int root = pq.top().idx;
  std::vector<std::string> codes(w.size());
  std::function<void(int, const std::string &)> gen = [&](int idx,
                                                         const std::string &pre) {
    const Node &nd = nodes[idx];
    if (nd.left < 0 && nd.right < 0) {
      if (nd.leaf_index >= 0 &&
          nd.leaf_index < static_cast<int>(codes.size()))
        codes[nd.leaf_index] = pre.empty() ? "0" : pre;
      return;
    }
    if (nd.left >= 0)
      gen(nd.left, pre + "0");
    if (nd.right >= 0)
      gen(nd.right, pre + "1");
  };
  gen(root, "");

  out << "叶结点编码（原始输入索引，权值，码字）：\n";
  const int orig_n = static_cast<int>(leaf_weights.size());
  for (int i = 0; i < orig_n; ++i)
    out << "  叶 " << i << " 权=" << leaf_weights[i] << " 码=" << codes[i]
        << "\n";
  out << "说明：已用 0 权虚叶补足使可反复两两合并；Huffman 树为满二叉树结构。\n";
  return out.str();
}

std::vector<long long> ford_bellman(int start,
                                    const std::vector<std::vector<int>> &graph,
                                    bool &has_negative_cycle) {
  has_negative_cycle = false;
  const int n = static_cast<int>(graph.size());
  const long long INF = std::numeric_limits<long long>::max() / 4;
  std::vector<long long> dist(n, INF);
  if (start < 0 || start >= n)
    return dist;
  dist[start] = 0;

  for (int it = 0; it < n - 1; ++it) {
    bool upd = false;
    for (int u = 0; u < n; ++u) {
      if (dist[u] >= INF)
        continue;
      for (int v = 0; v < n; ++v) {
        if (graph[u][v] == 0)
          continue;
        const long long nd = dist[u] + graph[u][v];
        if (nd < dist[v]) {
          dist[v] = nd;
          upd = true;
        }
      }
    }
    if (!upd)
      break;
  }

  for (int u = 0; u < n; ++u) {
    if (dist[u] >= INF)
      continue;
    for (int v = 0; v < n; ++v) {
      if (graph[u][v] == 0)
        continue;
      if (dist[u] + graph[u][v] < dist[v]) {
        has_negative_cycle = true;
        return dist;
      }
    }
  }
  return dist;
}

std::vector<int> tsp_DFS(const incidence_matrix &input) {
  std::vector<std::vector<int>> ad = input.in_to_ad();
  const int n = static_cast<int>(ad.size());
  if (n == 0)
    return {};
  std::vector<int> path;
  std::vector<int> best;
  std::function<bool(int, int)> dfs = [&](int u, int depth) -> bool {
    path.push_back(u);
    if (depth == n) {
      if (u < n && ad[u][0]) {
        path.push_back(0);
        best = path;
        path.pop_back();
        path.pop_back();
        return true;
      }
      path.pop_back();
      return false;
    }
    for (int v = 0; v < n; ++v) {
      if (!ad[u][v])
        continue;
      if (std::find(path.begin(), path.end(), v) != path.end())
        continue;
      if (dfs(v, depth + 1))
        return true;
    }
    path.pop_back();
    return false;
  };

  if (dfs(0, 1))
    return best;
  return {};
}