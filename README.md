# basic_graph_algorithm

本仓库包含两部分：

- **C++**：图与矩阵相关算法（邻接矩阵、关联矩阵、Warshall、Hamilton、Dijkstra、Kruskal、Huffman、Ford–Bellman、旅行商 DFS、生成树与矩阵运算等）
- **前端**：离线可视化页面（`web/`），支持「图 / 树」两种输入模式与多类算法输出

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

## 约定（有权图与 Ford）

在 **`graph_algorithm` 中带权邻接矩阵**的用法与 **网页图模式** 中 Dijkstra / Ford 等一致：

- **`matrix[i][j] == 0`** 表示 **没有从 i 到 j 的有向边**
- **`matrix[i][j] != 0`** 表示 **有向边，权值为该整数（可为负，用于 Ford–Bellman）**

因此 **边权为 0** 的有向边无法用本约定表示；若图中出现 **负权边**，语义上应 **仅使用 Ford–Bellman**（网页会在检测到负权时自动关闭 Warshall、Dijkstra、Hamilton、Kruskal、TSP 等选项）。

**无向图 / Kruskal**（与其它算法区分）：在 **仅 Kruskal** 使用的无向邻接表示中，`und_adj[i][j]`（仅 `i < j`）为 **`kruskal_no_edge()`（即 `INT_MAX`）** 表示 **无边**，**其它任意整数（含 0 与负数）** 均为边权；**自环**（对角线）不参与，亦勿依赖对角线填权。网页在有向表上会先 **无向化**（无弧对记为 `KRUSKAL_NO_EDGE`，双边取 `min` 等），再跑 Kruskal；**树模式**仍以 **0 表示无棱**，再映射为哨兵值。

## C++ 构建与运行（CMake）

```bash
cmake -S . -B build
cmake --build build
```

可执行文件为 `graph_algorithm_cli`（位于构建目录），运行后按程序内说明或 `main.cpp` 中的示例逻辑使用。

也可在本机用支持 **C++17** 的编译器直接编译 `src/` 与 `include/`。

## `graph_algorithm.h` 中与图算法相关的主要接口

| 接口 | 说明 |
|------|------|
| `warshall` | 可达性（传递闭包），重载支持按步模拟 |
| `hamilton` | Hamilton 道路搜索（0/1 邻接意义） |
| `dijkstra` | 单源最短路（仅对 **正权边** 松弛，`0` 为无边） |
| `kruskal_no_edge()` | 返回 Kruskal 用的「无边」哨兵（`INT_MAX`） |
| `kruskal_mst` | 无向最小生成树：仅扫 `i<j`，跳过 `kruskal_no_edge()`，**允许 0 权边**；不连通时 `connected == false` 并清空边列表 |
| `huffman_complete_binary_report` | Huffman：输入叶权（非负、至少两叶、权和为正），返回建合并过程与码字的字符串说明（含 0 权虚叶补足） |
| `ford_bellman` | 单源最短路；`has_negative_cycle` 表示从起点可达负回路 |
| `tsp_DFS` | 基于 **关联矩阵** 转 0/1 无向拓扑，DFS 求一条 **Hamilton 回路**（从 `v0` 出发并回到 `v0`） |

示例调用见 `src/main.cpp` 中「Ford / Kruskal / Huffman / TSP」相关段落。

## 网页可视化

### 运行方式

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

### 输入类型

- **图**：有向邻接矩阵；允许负权；出现负权时 **仅可选用 Ford / Bellman-Ford**（及「仅展示」）。
- **树**：无向、矩阵对称、边权均非负；**恰好 `n−1` 条正权无向边**且连通；**对角线** 可填非负整数作为 **Huffman 叶权**（边权仍在 `i≠j`）。

### 图模式下可选算法（无负权时）

仅展示、Warshall、Dijkstra、Hamilton、Kruskal、旅行商 TSP（DFS 最小权回路）、Ford。

### 树模式下可选算法

仅展示、生成树数量（矩阵树定理）、Kruskal、Huffman（对角线为叶权）。

### 离线打包

在 `web` 目录运行或双击 `release.bat`，会生成 `web/dist/graph_visualization_offline.zip`。

---

## 其它 C++ 功能（`matrix` 与 `tree`）

### 1. `tree` 类

用于生成树相关结构，包括：

- 基本回路矩阵、基本割集矩阵
- 生成树数量（矩阵树定理）、有根树数量

使用示例：

```cpp
std::vector<std::vector<int>> graph_data = {
  {1, 1, 0},
  {-1, 0, 1},
  {0, -1, -1}
};
incidence_matrix graph(graph_data);

std::vector<std::vector<int>> tree_data = {
  {1, 1},
  {-1, 0},
  {0, -1}
};
incidence_matrix tree_mat(tree_data);

tree t(graph, tree_mat);
incidence_matrix circuit = t.get_basic_circuit_matrix();
circuit.print();

incidence_matrix cut_set = t.get_basic_cut_set_matrix();
cut_set.print();

int tree_num = t.get_tree_num();
int root_tree_num = t.get_root_tree_num(0);
```

### 2. `matrix_operations` 命名空间

列交换、行最简形、矩阵分割、转置、取负、行列式、矩阵乘法等。

### 3. 环检测

`incidence_matrix::find_cycle()`：检测无向意义下的环并返回顶点序列示例。

```cpp
incidence_matrix g(graph_data);
std::vector<int> cycle = g.find_cycle();
```

---

## 关联矩阵 API 说明

`incidence_matrix::in_to_ad()` 为 **const** 成员函数，从关联矩阵得到 0/1 无向邻接（用于 `tsp_DFS` 等）。
