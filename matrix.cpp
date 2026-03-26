#include "matrix.h"
#include <climits>

std::vector<int> &adjacency_matrix::operator[](int row) { return data[row]; }

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