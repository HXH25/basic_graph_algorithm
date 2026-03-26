# 图算法可视化（离线版）

这是一个可离线运行的图算法可视化程序，包含：

- 图输入：先输入节点数，再在矩阵表格中填写邻接矩阵
- 算法：`Warshall`、`Dijkstra`、`Hamilton`
- 图渲染：节点编号、边编号、边权、单向/双向箭头
- 矩阵联动：邻接矩阵、关联矩阵展示
- 节点交互：点击节点可高亮相关边与邻接矩阵行列

## 目录结构

```text
web/
├─ index.html
├─ style.css
├─ app.js
├─ tsinghua_logo.png
├─ run_offline.py
└─ start_offline.bat
```

## 离线运行（Windows）

### 方法 1（一键启动）

双击 `start_offline.bat`

- 自动启动本地服务
- 自动打开浏览器
- 默认地址：`http://127.0.0.1:8000`
- 停止程序：回到命令行窗口按 `Ctrl + C`

### 方法 2（命令行）

```bash
cd web
python run_offline.py
```

可选参数：

```bash
python run_offline.py --host 127.0.0.1 --port 8000 --no-browser
```

## 一键打包（可分发 zip）

在 `web` 目录双击：

- `release.bat`

脚本会生成：

- `web/dist/graph_visualization_offline.zip`

把这个 zip 发给别人后，对方解压并双击 `start_offline.bat` 就能离线使用。