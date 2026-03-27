#pragma once
#include <iostream>
#include <vector>

// 矩阵处理函数声明
namespace matrix_operations {
    // 1. 列交换
    void swap_columns(std::vector<std::vector<int>>& matrix, int col1, int col2);
    
    // 2. 只通过初等行变换把一个矩阵变为rref（行最简形）
    void to_rref(std::vector<std::vector<int>>& matrix);
    
    // 3. 把一个rref矩阵分成单位阵和其他部分
    std::pair<std::vector<std::vector<int>>, std::vector<std::vector<int>>> split_rref(const std::vector<std::vector<int>>& matrix);
    
    // 4. 把一个矩阵转置
    std::vector<std::vector<int>> transpose(const std::vector<std::vector<int>>& matrix);
    
    // 5. 把一个矩阵的所有矩阵元全变成负数
    std::vector<std::vector<int>> negate(const std::vector<std::vector<int>>& matrix);
    
    // 6. 求一个矩阵的行列式
    int determinant(const std::vector<std::vector<int>>& matrix);
    
    // 7. 求两个矩阵的乘法
    std::vector<std::vector<int>> multiply(const std::vector<std::vector<int>>& matrix1, const std::vector<std::vector<int>>& matrix2);
}

class adjacency_matrix // 邻接矩阵
{
private:
  std::vector<std::vector<int>> data;

public:
  std::vector<std::vector<int>> ad_to_in();
  std::vector<std::vector<int>> matrix_to_list();

  // 重载[]运算符
  std::vector<int> &operator[](int row);

  adjacency_matrix();
  adjacency_matrix(int size_row, int size_column);
  adjacency_matrix(const std::vector<std::vector<int>> &matrix);

  int size();
  void print();
  std::vector<int> find_cycle();
  bool dfs_cycle(std::vector<std::vector<int>>& adj, int current, 
                 std::vector<bool>& visited, std::vector<int>& parent, 
                 std::vector<int>& cycle);
};

class incidence_matrix // 关联矩阵
{
private:
  std::vector<std::vector<int>> data;

public:
  // 构造函数
  incidence_matrix();
  incidence_matrix(int size_row, int size_column);
  incidence_matrix(const std::vector<std::vector<int>> &matrix);

  // 重载[]运算符
  std::vector<int> &operator[](int row);

  std::vector<std::vector<int>> in_to_ad();

  int size();
  void print();
  std::vector<int> find_cycle();
  bool dfs_cycle(std::vector<std::vector<int>>& adj, int current, 
                 std::vector<bool>& visited, std::vector<int>& parent, 
                 std::vector<int>& cycle);
  std::vector<std::vector<int>> get_data();
};

class adjacency_list // 邻接表
{
private:
  std::vector<std::vector<int>> data;

public:
  std::vector<std::vector<int>> list_to_matrix();
  adjacency_list();
  adjacency_list(int size_row);
  adjacency_list(const std::vector<std::vector<int>> &matrix);
  std::vector<int> &operator[](int row);
  int size();
  void print();
};

class tree
{
private:
  incidence_matrix graph_matrix;
  incidence_matrix tree_matrix;
  incidence_matrix basic_circuit_matrix;
  incidence_matrix basic_cut_set_matrix;
public:
  tree();
  tree(incidence_matrix graph_matrix, incidence_matrix tree_matrix);
  incidence_matrix get_tree_matrix();
  incidence_matrix get_basic_circuit_matrix();
  incidence_matrix get_basic_cut_set_matrix();
  int size();
  int get_tree_num();
  int get_root_tree_num(int root);
};