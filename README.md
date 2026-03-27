# basic_graph_algorithm

本仓库包含两部分：

- C++ 图算法实现（邻接矩阵/关联矩阵、Warshall、Hamilton、Dijkstra）
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
