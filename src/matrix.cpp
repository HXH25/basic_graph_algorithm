#include "matrix.h"
#include <climits>
#include <algorithm>
#include <queue>
#include <numeric>
#include <algorithm>
#include <queue>

std::vector<int> &adjacency_matrix::operator[](int row) { return data[row]; }

// 矩阵处理函数实现
namespace matrix_operations {
    // 1. 列交换
    void swap_columns(std::vector<std::vector<int>>& matrix, int col1, int col2) {
        if (matrix.empty() || col1 < 0 || col2 < 0 || col1 >= matrix[0].size() || col2 >= matrix[0].size()) {
            return;
        }
        for (size_t i = 0; i < matrix.size(); ++i) {
            std::swap(matrix[i][col1], matrix[i][col2]);
        }
    }
    
    // 2. 只通过初等行变换把一个矩阵变为rref（行最简形）
    void to_rref(std::vector<std::vector<int>>& matrix) {
        if (matrix.empty()) return;
        
        int rows = matrix.size();
        int cols = matrix[0].size();
        int lead = 0;
        
        for (int r = 0; r < rows; ++r) {
            if (lead >= cols) break;
            
            // 找到主元行
            int i = r;
            while (i < rows && matrix[i][lead] == 0) {
                ++i;
            }
            
            if (i == rows) {
                ++lead;
                --r;
                continue;
            }
            
            // 交换行
            std::swap(matrix[r], matrix[i]);
            
            // 归一化主元行
            int pivot = matrix[r][lead];
            for (int c = lead; c < cols; ++c) {
                matrix[r][c] /= pivot;
            }
            
            // 消去其他行
            for (int i = 0; i < rows; ++i) {
                if (i != r && matrix[i][lead] != 0) {
                    int factor = matrix[i][lead];
                    for (int c = lead; c < cols; ++c) {
                        matrix[i][c] -= factor * matrix[r][c];
                    }
                }
            }
            
            ++lead;
        }
    }
    
    // 3. 把一个rref矩阵分成单位阵和其他部分
    std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> split_rref(const std::vector<std::vector<int>>& matrix) {
        if (matrix.empty()) {
            return { {}, {} };
        }
        
        int rows = matrix.size();
        int cols = matrix[0].size();
        
        // 找出单位矩阵的大小
        int rank = 0;
        for (int r = 0; r < rows; ++r) {
            bool all_zero = true;
            for (int c = 0; c < cols; ++c) {
                if (matrix[r][c] != 0) {
                    all_zero = false;
                    break;
                }
            }
            if (!all_zero) {
                ++rank;
            }
        }
        
        // 构建单位矩阵部分
        std::vector<std::vector<int>> identity(rank, std::vector<int>(rank, 0));
        for (int r = 0; r < rank; ++r) {
            for (int c = 0; c < cols; ++c) {
                if (matrix[r][c] == 1) {
                    identity[r][c] = 1;
                    break;
                }
            }
        }
        
        // 构建其他部分
        std::vector<std::vector<int>> other(rank, std::vector<int>(cols - rank, 0));
        for (int r = 0; r < rank; ++r) {
            int col_idx = 0;
            for (int c = 0; c < cols; ++c) {
                if (matrix[r][c] == 1) {
                    // 跳过单位矩阵部分
                    ++c;
                    while (c < cols) {
                        other[r][col_idx++] = matrix[r][c++];
                    }
                    break;
                }
            }
        }
        
        return { identity, other };
    }
    
    // 4. 把一个矩阵转置
    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& matrix) {
        if (matrix.empty()) {
            return {};
        }
        
        int rows = matrix.size();
        int cols = matrix[0].size();
        std::vector<std::vector<int>> result(cols, std::vector<int>(rows));
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result[j][i] = matrix[i][j];
            }
        }
        
        return result;
    }
    
    // 5. 把一个矩阵的所有矩阵元全变成负数
    std::vector<std::vector<int>> negate(const std::vector<std::vector<int>>& matrix) {
        if (matrix.empty()) {
            return {};
        }
        
        int rows = matrix.size();
        int cols = matrix[0].size();
        std::vector<std::vector<int>> result(rows, std::vector<int>(cols));
        
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                result[i][j] = -matrix[i][j];
            }
        }
        
        return result;
    }
    
    // 6. 求一个矩阵的行列式
    int determinant(const std::vector<std::vector<int>>& matrix) {
        int n = matrix.size();
        if (n != matrix[0].size()) {
            return 0; // 非方阵行列式无定义
        }
        
        if (n == 1) {
            return matrix[0][0];
        }
        
        if (n == 2) {
            return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
        }
        
        int det = 0;
        for (int j = 0; j < n; ++j) {
            // 计算代数余子式
            std::vector<std::vector<int>> minor(n - 1, std::vector<int>(n - 1));
            for (int i = 1; i < n; ++i) {
                int col = 0;
                for (int k = 0; k < n; ++k) {
                    if (k != j) {
                        minor[i - 1][col++] = matrix[i][k];
                    }
                }
            }
            
            int sign = (j % 2 == 0) ? 1 : -1;
            det += sign * matrix[0][j] * determinant(minor);
        }
        
        return det;
    }
    
    // 7. 求两个矩阵的乘法
    std::vector<std::vector<int>> multiply(const std::vector<std::vector<int>>& matrix1, const std::vector<std::vector<int>>& matrix2) {
        if (matrix1.empty() || matrix2.empty() || matrix1[0].size() != matrix2.size()) {
            return {}; // 矩阵维度不匹配
        }
        
        int rows1 = matrix1.size();
        int cols1 = matrix1[0].size();
        int cols2 = matrix2[0].size();
        
        std::vector<std::vector<int>> result(rows1, std::vector<int>(cols2, 0));
        
        for (int i = 0; i < rows1; ++i) {
            for (int j = 0; j < cols2; ++j) {
                for (int k = 0; k < cols1; ++k) {
                    result[i][j] += matrix1[i][k] * matrix2[k][j];
                }
            }
        }
        
        return result;
    }
}

adjacency_matrix::adjacency_matrix() {}

adjacency_matrix::adjacency_matrix(
    int size_row, int size_column) // size_row行size_column列的数组
{
  data.resize(size_row, std::vector<int>(size_column, 0));
}

adjacency_matrix::adjacency_matrix(
    const std::vector<std::vector<int>> &matrix) {
  data = matrix;
}

std::vector<std::vector<int>> adjacency_matrix::ad_to_in() {
  std::vector<std::vector<int>> target_matrix(data.size(),
                                              std::vector<int>(data.size(), 0));
  for (int i = 0; i < data[0].size(); i++) {
    int a = INT_MAX, b = INT_MAX;
    for (int j = 0; j < data.size(); j++) {
      if (data[j][i] != 0) {
        if (a == INT_MAX) {
          a = j;
        } else {
          b = j;
        }
      }
    }

    target_matrix[a][b] = 1;
    target_matrix[b][a] = 1;
  }

  return target_matrix;
}

// incidence_matrix 类的实现

// 默认构造函数
incidence_matrix::incidence_matrix() {}

// 带参数的构造函数
incidence_matrix::incidence_matrix(int size_row, int size_column) {
  data.resize(size_row, std::vector<int>(size_column, 0));
}

// 从二维数组创建的构造函数
incidence_matrix::incidence_matrix(
    const std::vector<std::vector<int>> &matrix) {
  data = matrix;
}

// 重载[]运算符
std::vector<int> &incidence_matrix::operator[](int row) { return data[row]; }

// 关联矩阵转邻接矩阵
std::vector<std::vector<int>> incidence_matrix::in_to_ad() {
  // 关联矩阵的行数是顶点数，列数是边数
  int num_vertices = data.size();
  std::vector<std::vector<int>> target_matrix(
      num_vertices, std::vector<int>(num_vertices, 0));

  // 遍历每一条边（列）
  for (int i = 0; i < data[0].size(); i++) {
    int a = -1, b = -1;
    // 遍历每一个顶点（行）
    for (int j = 0; j < data.size(); j++) {
      if (data[j][i] != 0) {
        if (a == -1) {
          a = j;
        } else {
          b = j;
        }
      }
    }

    // 在邻接矩阵中标记边
    if (a != -1 && b != -1) {
      target_matrix[a][b] = 1;
      target_matrix[b][a] = 1;
    }
  }

  return target_matrix;
}

std::vector<std::vector<int>> incidence_matrix::get_data() { return data; }

int adjacency_matrix::size() { return data.size(); }

int incidence_matrix::size() { return data.size(); }

void adjacency_matrix::print() {
  for (int i = 0; i < data.size(); ++i) {
    for (int j = 0; j < data[i].size(); ++j) {
      std::cout << data[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void incidence_matrix::print() {
  for (int i = 0; i < data.size(); ++i) {
    for (int j = 0; j < data[i].size(); ++j) {
      std::cout << data[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

// 检测环并返回环的路径
std::vector<int> incidence_matrix::find_cycle() {
  int num_vertices = data.size();
  if (num_vertices == 0) return {};
  
  // 将关联矩阵转换为邻接表
  std::vector<std::vector<int>> adj(num_vertices);
  for (int edge = 0; edge < data[0].size(); edge++) {
    int u = -1, v = -1;
    for (int vertex = 0; vertex < num_vertices; vertex++) {
      if (data[vertex][edge] != 0) {
        if (u == -1) {
          u = vertex;
        } else {
          v = vertex;
        }
      }
    }
    if (u != -1 && v != -1) {
      adj[u].push_back(v);
      adj[v].push_back(u);
    }
  }
  
  // 使用DFS检测环
  std::vector<bool> visited(num_vertices, false);
  std::vector<int> parent(num_vertices, -1);
  std::vector<int> cycle;
  
  for (int i = 0; i < num_vertices; i++) {
    if (!visited[i]) {
      if (dfs_cycle(adj, i, visited, parent, cycle)) {
        // 反转路径以获得正确的环顺序
        std::reverse(cycle.begin(), cycle.end());
        return cycle;
      }
    }
  }
  
  return {}; // 没有找到环
}

// DFS辅助函数，用于检测环
bool incidence_matrix::dfs_cycle(std::vector<std::vector<int>>& adj, int current, 
                               std::vector<bool>& visited, std::vector<int>& parent, 
                               std::vector<int>& cycle) {
  visited[current] = true;
  
  for (int neighbor : adj[current]) {
    if (!visited[neighbor]) {
      parent[neighbor] = current;
      if (dfs_cycle(adj, neighbor, visited, parent, cycle)) {
        return true;
      }
    } else if (neighbor != parent[current]) {
      // 找到环，从neighbor回溯到current
      int node = current;
      while (node != neighbor) {
        cycle.push_back(node);
        node = parent[node];
      }
      cycle.push_back(neighbor);
      cycle.push_back(current);
      return true;
    }
  }
  
  return false;
}

// 邻接矩阵转邻接表
std::vector<std::vector<int>> adjacency_matrix::matrix_to_list() {
  std::vector<std::vector<int>> list(data.size());
  for (int i = 0; i < data.size(); i++) {
    for (int j = 0; j < data[i].size(); j++) {
      if (data[i][j] != 0) {
        list[i].push_back(j);
      }
    }
  }
  return list;
}

// adjacency_list 类的实现

// 默认构造函数
adjacency_list::adjacency_list() {}

// 带参数的构造函数
adjacency_list::adjacency_list(int size_row) { data.resize(size_row); }

// 从二维数组创建的构造函数
adjacency_list::adjacency_list(const std::vector<std::vector<int>> &matrix) {
  data = matrix;
}

// 重载[]运算符
std::vector<int> &adjacency_list::operator[](int row) { return data[row]; }

// 邻接表转邻接矩阵
std::vector<std::vector<int>> adjacency_list::list_to_matrix() {
  // 首先确定矩阵的大小
  int n = data.size();
  std::vector<std::vector<int>> matrix(n, std::vector<int>(n, 0));

  for (int i = 0; i < n; i++) {
    for (int j : data[i]) {
      matrix[i][j] = 1;
    }
  }

  return matrix;
}

// size方法
int adjacency_list::size() { return data.size(); }

// print方法
void adjacency_list::print() {
  for (int i = 0; i < data.size(); ++i) {
    std::cout << i << ": ";
    for (int j : data[i]) {
      std::cout << j << " ";
    }
    std::cout << std::endl;
  }
}

//下面是tree类的实现
tree::tree() {}

tree::tree(incidence_matrix graph_matrix, incidence_matrix tree_matrix) {
  this->graph_matrix = graph_matrix;
  this->tree_matrix = tree_matrix;

  //根据树初始化基本回路矩阵和基本割集矩阵
  //首先找到并存储余树枝的位置
  std::vector<int> extra_edge_positions = {};
  
  //识别余树枝
  // 遍历图的所有边（列）
  int num_edges = graph_matrix[0].size();
  int num_tree_edges = tree_matrix[0].size();
  
  // 创建边是否在树中的映射
  std::vector<bool> is_tree_edge(num_edges, false);
  
  // 遍历树矩阵的所有边（列）
  for (int edge = 0; edge < num_tree_edges; edge++) {
    // 检查这条边是否存在（至少有一个非零值）
    bool edge_exists = false;
    for (int vertex = 0; vertex < tree_matrix.size(); vertex++) {
      if (tree_matrix[vertex][edge] != 0) {
        edge_exists = true;
        break;
      }
    }
    if (edge_exists) {
      // 找到对应的图中的边
      for (int graph_edge = 0; graph_edge < num_edges; graph_edge++) {
        bool edges_match = true;
        for (int vertex = 0; vertex < tree_matrix.size(); vertex++) {
          if (tree_matrix[vertex][edge] != graph_matrix[vertex][graph_edge]) {
            edges_match = false;
            break;
          }
        }
        if (edges_match) {
          is_tree_edge[graph_edge] = true;
          break;
        }
      }
    }
  }
  
  // 收集余树枝
  for (int edge = 0; edge < num_edges; edge++) {
    if (!is_tree_edge[edge]) {
      extra_edge_positions.push_back(edge);
    }
  }
  
  int extra_edge_num = extra_edge_positions.size();

  //初始化基本回路矩阵
  // 行数：余树枝数量，列数：图的边数
  this->basic_circuit_matrix = incidence_matrix(extra_edge_num, num_edges);
  
  for(int i = 0; i < extra_edge_positions.size(); i++)
  {
    int chord = extra_edge_positions[i];
    this->basic_circuit_matrix[i][chord] = 1;
    
    // 找每个余树枝所在的环
    // 构建临时树矩阵，包含树的所有边和当前余树枝
    incidence_matrix temp_tree = tree_matrix;
    
    // 为临时树添加余树枝
    for(int j = 0; j < tree_matrix.size(); j++)
    {
      temp_tree[j].push_back(graph_matrix[j][chord]);
    }
    
    // 找到环
    std::vector<int> cycle = temp_tree.find_cycle();
    
    if(cycle.size() > 0)
    {
      // 处理环的顶点路径
      // 遍历环的顶点，找到对应的边
      for(int j = 0; j < cycle.size() - 1; j++)
      {
        int u = cycle[j];
        int v = cycle[j+1];
        
        // 找到连接u和v的边
        for (int edge = 0; edge < num_edges; edge++) {
          bool connects_u = (graph_matrix[u][edge] == 1 || graph_matrix[u][edge] == -1);
          bool connects_v = (graph_matrix[v][edge] == 1 || graph_matrix[v][edge] == -1);
          
          if (connects_u && connects_v) {
            // 检查这条边是否是树边
            if (is_tree_edge[edge]) {
              this->basic_circuit_matrix[i][edge] = 1;
            }
            break;
          }
        }
      }
    }
  }
  // 构建基本割集矩阵
  // 1. 首先将基本回路矩阵转换为vector<vector<int>>
  int circuit_rows = this->basic_circuit_matrix.size();
  int circuit_cols = num_edges;
  std::vector<std::vector<int>> circuit_matrix(circuit_rows, std::vector<int>(circuit_cols));
  for (int i = 0; i < circuit_rows; i++) {
    for (int j = 0; j < circuit_cols; j++) {
      circuit_matrix[i][j] = this->basic_circuit_matrix[i][j];
    }
  }
  
  // 2. 对基本回路矩阵进行列交换，将余树枝移到前面
  for(int i = 0; i < extra_edge_num; i++)
  {
    matrix_operations::swap_columns(circuit_matrix, i, extra_edge_positions[i]);
  }
  
  // 3. 将基本回路矩阵转换为行最简形
  matrix_operations::to_rref(circuit_matrix);
  
  // 4. 分割行最简形矩阵为单位阵和其他部分
  auto temp_pair = matrix_operations::split_rref(circuit_matrix);
  
  // 4. 对其他部分进行转置和取负
  auto transposed = matrix_operations::transpose(temp_pair.second);
  auto negated = matrix_operations::negate(transposed);
  
  // 5. 构建基本割集矩阵
  // 基本割集矩阵的结构为 [negated | identity]
  int num_tree_edges = num_edges - extra_edge_num;
  std::vector<std::vector<int>> cut_set_matrix(num_tree_edges, std::vector<int>(num_edges, 0));
  
  // 填充前 extra_edge_num 列
  for (int i = 0; i < num_tree_edges; i++) {
    for (int j = 0; j < extra_edge_num; j++) {
      cut_set_matrix[i][j] = negated[i][j];
    }
  }
  
  // 填充后 num_tree_edges 列（单位矩阵）
  for (int i = 0; i < num_tree_edges; i++) {
    cut_set_matrix[i][extra_edge_num + i] = 1;
  }
  
  // 6. 恢复列的原始顺序
  std::vector<int> original_order(num_edges);
  std::iota(original_order.begin(), original_order.end(), 0);
  
  // 首先记录当前列的顺序
  std::vector<int> current_order(num_edges);
  for (int i = 0; i < extra_edge_num; i++) {
    current_order[i] = extra_edge_positions[i];
  }
  
  int idx = extra_edge_num;
  for (int i = 0; i < num_edges; i++) {
    bool is_chord = false;
    for (int j = 0; j < extra_edge_num; j++) {
      if (i == extra_edge_positions[j]) {
        is_chord = true;
        break;
      }
    }
    if (!is_chord) {
      current_order[idx++] = i;
    }
  }
  
  // 构建逆置换
  std::vector<int> inverse_perm(num_edges);
  for (int i = 0; i < num_edges; i++) {
    inverse_perm[current_order[i]] = i;
  }
  
  // 应用逆置换，恢复原始列顺序
  std::vector<std::vector<int>> final_cut_set(num_tree_edges, std::vector<int>(num_edges, 0));
  for (int i = 0; i < num_tree_edges; i++) {
    for (int j = 0; j < num_edges; j++) {
      final_cut_set[i][j] = cut_set_matrix[i][inverse_perm[j]];
    }
  }
  
  // 7. 设置基本割集矩阵
  this->basic_cut_set_matrix = incidence_matrix(final_cut_set);
}

incidence_matrix tree::get_tree_matrix()
{
  return tree_matrix;
}

incidence_matrix tree::get_basic_circuit_matrix()
{
  return basic_circuit_matrix;
}

incidence_matrix tree::get_basic_cut_set_matrix()
{
  return basic_cut_set_matrix;
}

int tree::size()
{
  return tree_matrix.size();
}

int tree::get_tree_num()
{
  // 使用矩阵树定理计算生成树的数量
  
  // 1. 将关联矩阵转换为邻接矩阵
  int num_vertices = graph_matrix.size();
  std::vector<std::vector<int>> adj_matrix(num_vertices, std::vector<int>(num_vertices, 0));
  
  // 遍历所有边
  int num_edges = graph_matrix[0].size();
  for (int edge = 0; edge < num_edges; edge++) {
    int u = -1, v = -1;
    for (int vertex = 0; vertex < num_vertices; vertex++) {
      if (graph_matrix[vertex][edge] != 0) {
        if (u == -1) {
          u = vertex;
        } else {
          v = vertex;
        }
      }
    }
    if (u != -1 && v != -1) {
      adj_matrix[u][v] = 1;
      adj_matrix[v][u] = 1;
    }
  }
  
  // 2. 计算度矩阵
  std::vector<std::vector<int>> degree_matrix(num_vertices, std::vector<int>(num_vertices, 0));
  for (int i = 0; i < num_vertices; i++) {
    int degree = 0;
    for (int j = 0; j < num_vertices; j++) {
      degree += adj_matrix[i][j];
    }
    degree_matrix[i][i] = degree;
  }
  
  // 3. 计算拉普拉斯矩阵 L = D - A
  std::vector<std::vector<int>> laplacian(num_vertices, std::vector<int>(num_vertices, 0));
  for (int i = 0; i < num_vertices; i++) {
    for (int j = 0; j < num_vertices; j++) {
      laplacian[i][j] = degree_matrix[i][j] - adj_matrix[i][j];
    }
  }
  
  // 4. 去掉最后一行和最后一列，得到n-1阶主子式
  std::vector<std::vector<int>> laplacian_sub(num_vertices - 1, std::vector<int>(num_vertices - 1, 0));
  for (int i = 0; i < num_vertices - 1; i++) {
    for (int j = 0; j < num_vertices - 1; j++) {
      laplacian_sub[i][j] = laplacian[i][j];
    }
  }
  
  // 5. 计算行列式，即为生成树的数量
  return matrix_operations::determinant(laplacian_sub);
}

int tree::get_root_tree_num(int root)
{
  // 计算有根树的数量（指定根节点）
  
  // 1. 检查根节点是否有效
  int num_vertices = graph_matrix.size();
  if (root < 0 || root >= num_vertices) {
    return 0; // 无效的根节点
  }
  
  // 2. 将关联矩阵转换为邻接矩阵
  std::vector<std::vector<int>> adj_matrix(num_vertices, std::vector<int>(num_vertices, 0));
  
  // 遍历所有边
  int num_edges = graph_matrix[0].size();
  for (int edge = 0; edge < num_edges; edge++) {
    int u = -1, v = -1;
    for (int vertex = 0; vertex < num_vertices; vertex++) {
      if (graph_matrix[vertex][edge] != 0) {
        if (u == -1) {
          u = vertex;
        } else {
          v = vertex;
        }
      }
    }
    if (u != -1 && v != -1) {
      adj_matrix[u][v] = 1;
      adj_matrix[v][u] = 1;
    }
  }
  
  // 3. 计算度矩阵
  std::vector<std::vector<int>> degree_matrix(num_vertices, std::vector<int>(num_vertices, 0));
  for (int i = 0; i < num_vertices; i++) {
    int degree = 0;
    for (int j = 0; j < num_vertices; j++) {
      degree += adj_matrix[i][j];
    }
    degree_matrix[i][i] = degree;
  }
  
  // 4. 计算拉普拉斯矩阵 L = D - A
  std::vector<std::vector<int>> laplacian(num_vertices, std::vector<int>(num_vertices, 0));
  for (int i = 0; i < num_vertices; i++) {
    for (int j = 0; j < num_vertices; j++) {
      laplacian[i][j] = degree_matrix[i][j] - adj_matrix[i][j];
    }
  }
  
  // 5. 对于有根树，我们保留根节点的行和列
  // 去掉除根节点外的行和列
  int size = num_vertices - 1;
  std::vector<std::vector<int>> laplacian_sub(size, std::vector<int>(size, 0));
  
  int row = 0;
  for (int i = 0; i < num_vertices; i++) {
    if (i == root) continue;
    int col = 0;
    for (int j = 0; j < num_vertices; j++) {
      if (j == root) continue;
      laplacian_sub[row][col] = laplacian[i][j];
      col++;
    }
    row++;
  }
  
  // 6. 计算行列式，即为有根树的数量
  return matrix_operations::determinant(laplacian_sub);
}