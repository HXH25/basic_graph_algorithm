# basic_graph_algorithm

本仓库包含两部分：

- C++ 图算法实现（邻接矩阵/关联矩阵、Warshall、Hamilton、Dijkstra、生成树相关算法）
- 前端离线可视化页面（`web/`）

## 目录结构

```text
.
├─ CMakeLists.txt
├─ include/
│  ├─ graph_algorithm.h
│  └─ matrix.h
├─ src/
│  ├─ main.cpp
│  ├─ graph_algorithm.cpp
│  └─ matrix.cpp
├─ web/
│  ├─ index.html
│  ├─ style.css
│  ├─ app.js
│  ├─ run_offline.py
│  ├─ start_offline.bat
│  ├─ release.bat
│  └─ assets/
└─ releases/
```

## C++ 构建与运行（CMake）

```bash
cmake -S . -B build
cmake --build build
```

可执行文件为 `graph_algorithm_cli`，在构建目录下运行即可。

## 网页离线运行

```bash
cd web
python run_offline.py
```

默认地址：`http://127.0.0.1:8000`

可选参数：

```bash
python run_offline.py --host 127.0.0.1 --port 8000 --no-browser
```

Windows 可双击 `web/start_offline.bat`。

## 网页离线打包

在 `web` 目录运行或双击：

- `release.bat`

会生成：

- `web/dist/graph_visualization_offline.zip`

## 新增功能

### 1. tree 类

用于处理生成树相关的算法，包括：

- 基本回路矩阵和基本割集矩阵的构建
- 生成树数量的计算（矩阵树定理）
- 有根树数量的计算

#### 使用示例

```cpp
// 创建图的关联矩阵
std::vector<std::vector<int>> graph_data = {
  {1, 1, 0},
  {-1, 0, 1},
  {0, -1, -1}
};
incidence_matrix graph(graph_data);

// 创建树的关联矩阵
std::vector<std::vector<int>> tree_data = {
  {1, 1},
  {-1, 0},
  {0, -1}
};
incidence_matrix tree(tree_data);

// 创建tree对象
tree t(graph, tree);

// 获取基本回路矩阵
incidence_matrix circuit = t.get_basic_circuit_matrix();
circuit.print();

// 获取基本割集矩阵
incidence_matrix cut_set = t.get_basic_cut_set_matrix();
cut_set.print();

// 计算生成树数量
int tree_num = t.get_tree_num();
std::cout << "生成树数量: " << tree_num << std::endl;

// 计算以顶点0为根的有根树数量
int root_tree_num = t.get_root_tree_num(0);
std::cout << "有根树数量: " << root_tree_num << std::endl;
```

### 2. 矩阵处理函数

提供了多种矩阵操作功能：

- 列交换
- 行最简形转换
- 矩阵分割
- 矩阵转置
- 矩阵取负
- 行列式计算
- 矩阵乘法

这些函数位于 `matrix_operations` 命名空间中。

### 3. 环检测

`incidence_matrix` 类新增了 `find_cycle()` 方法，用于检测图中的环并返回环的路径。

#### 使用示例

```cpp
// 创建图的关联矩阵
std::vector<std::vector<int>> graph_data = {
  {1, 1, 0},
  {-1, 0, 1},
  {0, -1, -1}
};
incidence_matrix graph(graph_data);

// 检测环
std::vector<int> cycle = graph.find_cycle();
if (!cycle.empty()) {
  std::cout << "找到环: ";
  for (int v : cycle) {
    std::cout << v << " ";
  }
  std::cout << std::endl;
} else {
  std::cout << "图中没有环" << std::endl;
}
```