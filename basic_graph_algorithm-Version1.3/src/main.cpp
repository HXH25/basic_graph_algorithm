#include "graph_algorithm.h"
#include "matrix.h"
#include <climits>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
  int n = 0;
  cin >> n;
  vector<vector<int>> input_matrix = {};
  for (int i = 0; i < n; ++i)
  {
    vector<int> row = {};
    for (int j = 0; j < n; ++j)
    {
      int input_int = 0;
      cin >> input_int;
      row.push_back(input_int);
    }
    input_matrix.push_back(row);
  } // 所有的算法都是基于邻接矩阵的

  // warshall算法使用示例:
  cout << endl;
  cout << "warshall:" << endl;
  adjacency_matrix out_matrix_warshall = warshall(input_matrix);
  out_matrix_warshall.print();

  // hamilton算法使用示例(寻找hamilton道路):
  cout << endl;
  cout << "hamilton:" << endl;
  vector<vector<int>> out_matrix_hamilton = hamilton(input_matrix);
  for (int i = 0; i < out_matrix_hamilton.size(); i++)
  {
    for (int j = 0; j < out_matrix_hamilton[i].size(); j++)
    {
      cout << out_matrix_hamilton[i][j] << " ";
    }
    cout << endl;
  }

  // 分支定界法使用示例:

  // 便宜算法使用示例:

  // Dijkstra算法使用示例:
  cout << endl;
  cout << "Dijkstra:" << endl;
  input_matrix = {};
  for (int i = 0; i < n; ++i) // 这里输入的是有权矩阵
  {
    vector<int> row = {};
    for (int j = 0; j < n; ++j)
    {
      int input_int = 0;
      cin >> input_int;
      row.push_back(input_int);
    }
    input_matrix.push_back(row);
  }
  vector<int> out_matrix_dijkstra = dijkstra(0, input_matrix);
  for (int i = 0; i < out_matrix_dijkstra.size(); i++)
  {
    cout << out_matrix_dijkstra[i] << " ";
  }
  cout << endl;

  // Ford（Bellman-Ford）算法使用示例:
  {
    std::vector<std::vector<int>> g = {
        {0, 4, 0},
        {0, 0, 5},
        {0, -3, 0},
    };
    bool neg = false;
    auto fb = ford_bellman(0, g, neg);
    cout << "Ford-Bellman distances from 0:";
    if (neg)
      cout << " negative cycle";
    cout << endl;
    for (size_t i = 0; i < fb.size(); ++i)
      cout << "v" << i << ":" << fb[i] << " ";
    cout << endl;
  }

  // Kruskal 最小生成树示例（无边为 kruskal_no_edge()，允许 0 权边，不含自环）:
  {
    const int NO = kruskal_no_edge();
    std::vector<std::vector<int>> u = {
        {NO, 2, NO},
        {2, NO, 0},
        {NO, 0, NO},
    };
    auto k = kruskal_mst(u);
    if (!k.connected)
      cout << "Kruskal: 不连通" << endl;
    else
    {
      cout << "Kruskal MST weight=" << k.total_weight << endl;
      for (const auto &e : k.mst_edges)
        cout << " edge " << std::get<0>(e) << "-" << std::get<1>(e)
             << " w=" << std::get<2>(e) << endl;
    }
  }

  // Huffman 示例:
  cout << huffman_complete_binary_report({1, 2, 3, 4});

  // TSP（关联矩阵 DFS）示例 — 三角形回路:
  {
    std::vector<std::vector<int>> inc = {
        {1, 1, 0},
        {-1, 0, 1},
        {0, -1, -1},
    };
    incidence_matrix im(inc);
    auto tour = tsp_DFS(im);
    cout << "TSP DFS tour:";
    for (int v : tour)
      cout << " " << v;
    cout << endl;
  }

  // 最长路径算法使用示例:

  // 新增功能示例
  cout << endl;
  cout << "=== 新增功能示例 ===" << endl;

  // 1. 环检测示例
  cout << endl;
  cout << "环检测: " << endl;
  std::vector<std::vector<int>> graph_data = {
      {1, 1, 0},
      {-1, 0, 1},
      {0, -1, -1}};
  incidence_matrix graph(graph_data);
  std::vector<int> cycle = graph.find_cycle();
  if (!cycle.empty())
  {
    cout << "找到环: ";
    for (int v : cycle)
    {
      cout << v << " ";
    }
    cout << endl;
  }
  else
  {
    cout << "图中没有环" << endl;
  }

  // 2. 生成树相关算法示例
  cout << endl;
  cout << "生成树相关算法: " << endl;

  // 创建树的关联矩阵
  std::vector<std::vector<int>> tree_data = {
      {1, 1},
      {-1, 0},
      {0, -1}};
  incidence_matrix tree_matrix(tree_data);

  // 创建tree对象
  tree t(graph, tree_matrix);

  // 获取基本回路矩阵
  cout << "基本回路矩阵: " << endl;
  incidence_matrix circuit = t.get_basic_circuit_matrix();
  circuit.print();

  // 获取基本割集矩阵
  cout << "基本割集矩阵: " << endl;
  incidence_matrix cut_set = t.get_basic_cut_set_matrix();
  cut_set.print();

  // 计算生成树数量
  int tree_num = t.get_tree_num();
  cout << "生成树数量: " << tree_num << endl;

  // 计算有根树数量
  int root_tree_num = t.get_root_tree_num(0);
  cout << "以顶点0为根的有根树数量: " << root_tree_num << endl;

  // 3. 矩阵处理函数示例
  cout << endl;
  cout << "矩阵处理函数: " << endl;
  std::vector<std::vector<int>> matrix = {
      {1, 2, 3},
      {4, 5, 6},
      {7, 8, 9}};

  // 矩阵转置
  std::vector<std::vector<int>> transposed = matrix_operations::transpose(matrix);
  cout << "转置矩阵: " << endl;
  for (auto &row : transposed)
  {
    for (int val : row)
    {
      cout << val << " ";
    }
    cout << endl;
  }

  // 矩阵取负
  std::vector<std::vector<int>> negated = matrix_operations::negate(matrix);
  cout << "取负矩阵: " << endl;
  for (auto &row : negated)
  {
    for (int val : row)
    {
      cout << val << " ";
    }
    cout << endl;
  }

  return 0;
}