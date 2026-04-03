const nodeCountInput = document.getElementById("nodeCountInput");
const generateTableBtn = document.getElementById("generateTableBtn");
const matrixEditorContainer = document.getElementById("matrixEditorContainer");
const buildGraphBtn = document.getElementById("buildGraphBtn");
const inputError = document.getElementById("inputError");
const inputHelpGraph = document.getElementById("inputHelpGraph");
const inputHelpTree = document.getElementById("inputHelpTree");

const algorithmSection = document.getElementById("algorithmSection");
const resultSection = document.getElementById("resultSection");
const algoOutputSection = document.getElementById("algoOutputSection");
const algoOutput = document.getElementById("algoOutput");
const algorithmSelect = document.getElementById("algorithmSelect");
const startNodeLabel = document.getElementById("startNodeLabel");
const startNodeInput = document.getElementById("startNodeInput");
const runAlgorithmBtn = document.getElementById("runAlgorithmBtn");
const algorithmError = document.getElementById("algorithmError");

const graphSvg = document.getElementById("graphSvg");
const adjacencyMatrixContainer = document.getElementById("adjacencyMatrixContainer");
const incidenceMatrixContainer = document.getElementById("incidenceMatrixContainer");

const inputKindRadios = document.querySelectorAll('input[name="inputKind"]');

/** 与 C++ kruskal_no_edge() / INT_MAX 一致：Kruskal 表示「无边」 */
const KRUSKAL_NO_EDGE = 2147483647;

let currentMatrix = [];
let matrixEditorSize = 0;
let currentUndirected = false;
let currentHasNegative = false;
let currentInputKind = "graph";

function getInputKind() {
  const r = document.querySelector('input[name="inputKind"]:checked');
  return r ? r.value : "graph";
}

function updateInputHelp() {
  const k = getInputKind();
  inputHelpGraph.classList.toggle("hidden", k !== "graph");
  inputHelpTree.classList.toggle("hidden", k !== "tree");
}

function matrixHasNegativeEdge(matrix) {
  for (let i = 0; i < matrix.length; i += 1) {
    for (let j = 0; j < matrix.length; j += 1) {
      if (i !== j && matrix[i][j] < 0) return true;
    }
  }
  return false;
}

function validateTreeSymmetricNonneg(matrix) {
  const n = matrix.length;
  for (let i = 0; i < n; i += 1) {
    for (let j = 0; j < n; j += 1) {
      if (!Number.isInteger(matrix[i][j]) || matrix[i][j] < 0) {
        return { ok: false, msg: `树模式：v${i}→v${j} 须为非负整数。` };
      }
      if (matrix[i][j] !== matrix[j][i]) {
        return { ok: false, msg: `树模式：矩阵须对称（v${i}→v${j}=${matrix[i][j]}，v${j}→v${i}=${matrix[j][i]}）。` };
      }
    }
  }
  let edges = 0;
  for (let i = 0; i < n; i += 1) {
    for (let j = i + 1; j < n; j += 1) {
      if (matrix[i][j] > 0) edges += 1;
    }
  }
  if (n > 1 && edges !== n - 1) {
    return { ok: false, msg: `树应有 ${n - 1} 条边，当前无向边数为 ${edges}。` };
  }
  const seen = Array(n).fill(false);
  const q = [0];
  seen[0] = true;
  let cnt = 0;
  while (q.length) {
    const u = q.shift();
    cnt += 1;
    for (let v = 0; v < n; v += 1) {
      if (u !== v && matrix[u][v] > 0 && !seen[v]) {
        seen[v] = true;
        q.push(v);
      }
    }
  }
  if (cnt !== n) {
    return { ok: false, msg: "树模式：图不连通。" };
  }
  return { ok: true };
}

function rebuildAlgorithmOptions() {
  const v = algorithmSelect.value;
  algorithmSelect.innerHTML = "";
  const add = (val, text) => {
    const o = document.createElement("option");
    o.value = val;
    o.textContent = text;
    algorithmSelect.appendChild(o);
  };

  if (currentInputKind === "tree") {
    add("none", "仅展示树结构");
    add("tree_count", "生成树数量（矩阵树定理）");
    add("kruskal", "Kruskal（最短树 / MST）");
    add("huffman", "Huffman 树（对角线为叶权）");
    const allowed = ["none", "tree_count", "kruskal", "huffman"];
    algorithmSelect.value = allowed.includes(v) ? v : "none";
  } else {
    add("none", "仅展示图结构");
    if (!currentHasNegative) {
      add("warshall", "Warshall（可达闭包）");
      add("dijkstra", "Dijkstra（单源最短路）");
      add("hamilton", "Hamilton（路径搜索）");
      add("kruskal", "Kruskal（无向最小生成树）");
      add("tsp", "旅行商 TSP（DFS，最小权回路）");
    }
    add("ford", "Ford / Bellman-Ford（含负权）");
    const allow = new Set(["none", "ford"]);
    if (!currentHasNegative) {
      ["warshall", "dijkstra", "hamilton", "kruskal", "tsp"].forEach((x) => allow.add(x));
    }
    let next = v;
    if (!allow.has(next)) next = currentHasNegative ? "ford" : "none";
    algorithmSelect.value = next;
  }
  updateStartNodeVisibility();
}

function createMatrixEditor(size, preset) {
  matrixEditorContainer.innerHTML = "";
  const table = document.createElement("table");
  const thead = document.createElement("thead");
  const trHead = document.createElement("tr");
  trHead.appendChild(document.createElement("th"));
  for (let j = 0; j < size; j += 1) {
    const th = document.createElement("th");
    th.textContent = `v${j}`;
    trHead.appendChild(th);
  }
  thead.appendChild(trHead);
  table.appendChild(thead);

  const kind = getInputKind();
  const tbody = document.createElement("tbody");
  for (let i = 0; i < size; i += 1) {
    const tr = document.createElement("tr");
    const rowHeader = document.createElement("th");
    rowHeader.textContent = `v${i}`;
    tr.appendChild(rowHeader);
    for (let j = 0; j < size; j += 1) {
      const td = document.createElement("td");
      const input = document.createElement("input");
      input.type = "number";
      input.step = "1";
      input.className = "matrix-cell-input";
      input.dataset.row = String(i);
      input.dataset.col = String(j);
      if (kind === "tree") {
        input.min = "0";
      } else {
        input.removeAttribute("min");
      }
      input.value = preset ? String(preset[i][j]) : "0";
      td.appendChild(input);
      tr.appendChild(td);
    }
    tbody.appendChild(tr);
  }
  table.appendChild(tbody);
  matrixEditorContainer.appendChild(table);
  matrixEditorContainer.classList.remove("hidden");
  matrixEditorSize = size;
}

function readMatrixFromEditor() {
  if (matrixEditorSize <= 0) {
    throw new Error("请先生成矩阵表格。");
  }
  const kind = getInputKind();
  const matrix = Array.from({ length: matrixEditorSize }, () => Array(matrixEditorSize).fill(0));
  const inputs = matrixEditorContainer.querySelectorAll(".matrix-cell-input");
  for (const input of inputs) {
    const row = Number(input.dataset.row);
    const col = Number(input.dataset.col);
    const val = Number(input.value);
    if (Number.isNaN(val) || !Number.isInteger(val)) {
      throw new Error(`v${row} -> v${col} 须为整数。`);
    }
    if (kind === "tree" && val < 0) {
      throw new Error(`树模式：v${row} -> v${col} 必须为非负整数。`);
    }
    matrix[row][col] = val;
  }
  if (kind === "tree") {
    const chk = validateTreeSymmetricNonneg(matrix);
    if (!chk.ok) throw new Error(chk.msg);
  }
  return matrix;
}

function buildDirectedEdges(matrix) {
  const edges = [];
  for (let i = 0; i < matrix.length; i += 1) {
    for (let j = 0; j < matrix[i].length; j += 1) {
      const w = matrix[i][j];
      if (w !== 0) {
        edges.push({ id: `e${edges.length}`, from: i, to: j, weight: w });
      }
    }
  }
  return edges;
}

function buildUndirectedEdges(matrix) {
  const edges = [];
  const n = matrix.length;
  for (let i = 0; i < n; i += 1) {
    for (let j = i + 1; j < n; j += 1) {
      const w = matrix[i][j];
      if (w > 0) edges.push({ id: `e${edges.length}`, from: i, to: j, weight: w });
    }
  }
  return edges;
}

/** 无向化：用于 Kruskal（Dijkstra 约定 0 表示有向无边）；无向边权可为 0；无弧对应该用 KRUSKAL_NO_EDGE */
function toUndirectedForMst(matrix) {
  const n = matrix.length;
  const u = Array.from({ length: n }, () => Array(n).fill(KRUSKAL_NO_EDGE));
  for (let i = 0; i < n; i += 1) u[i][i] = KRUSKAL_NO_EDGE;
  for (let i = 0; i < n; i += 1) {
    for (let j = i + 1; j < n; j += 1) {
      const a = matrix[i][j];
      const b = matrix[j][i];
      let w;
      if (a === 0 && b === 0) w = KRUSKAL_NO_EDGE;
      else if (a !== 0 && b !== 0) w = Math.min(a, b);
      else w = a !== 0 ? a : b;
      if (w !== KRUSKAL_NO_EDGE) {
        u[i][j] = w;
        u[j][i] = w;
      }
    }
  }
  return u;
}

/** 树模式对称矩阵：0 表示两点间无棱；与 Dijkstra 图输入一致，再转为 Kruskal 哨兵矩阵 */
function symTreeToKruskalUndirected(sym) {
  const n = sym.length;
  const u = Array.from({ length: n }, () => Array(n).fill(KRUSKAL_NO_EDGE));
  for (let i = 0; i < n; i += 1) {
    u[i][i] = KRUSKAL_NO_EDGE;
    for (let j = i + 1; j < n; j += 1) {
      const w = sym[i][j];
      if (w === 0) continue;
      u[i][j] = w;
      u[j][i] = w;
    }
  }
  return u;
}

function getNodePositions(n, width, height) {
  const cx = width / 2;
  const cy = height / 2;
  const radius = Math.max(150, Math.min(width, height) * 0.36);
  const nodes = [];
  for (let i = 0; i < n; i += 1) {
    const angle = -Math.PI / 2 + (2 * Math.PI * i) / n;
    nodes.push({ id: i, x: cx + radius * Math.cos(angle), y: cy + radius * Math.sin(angle) });
  }
  return nodes;
}

function svgEl(name, attrs = {}) {
  const el = document.createElementNS("http://www.w3.org/2000/svg", name);
  Object.entries(attrs).forEach(([k, v]) => el.setAttribute(k, String(v)));
  return el;
}

function clearSvg() {
  while (graphSvg.firstChild) {
    graphSvg.removeChild(graphSvg.firstChild);
  }
}

function createMarkers() {
  const defs = svgEl("defs");
  const endMarker = svgEl("marker", {
    id: "arrow-end",
    viewBox: "0 0 10 6",
    markerWidth: 10,
    markerHeight: 10,
    refX: 9,
    refY: 3,
    orient: "auto",
    markerUnits: "userSpaceOnUse"
  });
  endMarker.appendChild(svgEl("path", { d: "M0,0 L0,6 L9,3 z", fill: "#2f3b52" }));
  defs.appendChild(endMarker);

  const startMarker = svgEl("marker", {
    id: "arrow-start",
    viewBox: "0 0 10 6",
    markerWidth: 10,
    markerHeight: 10,
    refX: 1,
    refY: 3,
    orient: "auto",
    markerUnits: "userSpaceOnUse"
  });
  startMarker.appendChild(svgEl("path", { d: "M9,0 L9,6 L0,3 z", fill: "#2f3b52" }));
  defs.appendChild(startMarker);

  graphSvg.appendChild(defs);
}

function drawGraph(matrix, undirected) {
  clearSvg();
  createMarkers();

  const width = 760;
  const height = 520;
  const nodes = getNodePositions(matrix.length, width, height);
  const edges = undirected ? buildUndirectedEdges(matrix) : buildDirectedEdges(matrix);
  const edgeMap = new Map(edges.map((e) => [`${e.from}-${e.to}`, e]));
  const usedPair = new Set();

  edges.forEach((edge) => {
    if (undirected) {
      const pairKey = `${Math.min(edge.from, edge.to)}-${Math.max(edge.from, edge.to)}`;
      if (usedPair.has(pairKey)) return;
      usedPair.add(pairKey);
    }

    const reverseKey = `${edge.to}-${edge.from}`;
    const pairKeyBoth = `${Math.min(edge.from, edge.to)}-${Math.max(edge.from, edge.to)}`;
    if (!undirected && edge.from !== edge.to && usedPair.has(`${pairKeyBoth}-both`)) return;

    const from = nodes[edge.from];
    const to = nodes[edge.to];
    const reverseEdge = edgeMap.get(reverseKey);
    const hasReverse = Boolean(reverseEdge) && !undirected;

    const dx = to.x - from.x;
    const dy = to.y - from.y;
    const len = Math.hypot(dx, dy) || 1;
    const ux = dx / len;
    const uy = dy / len;
    const px = -uy;
    const py = ux;

    const nodeRadius = 20;
    const startX = from.x + ux * nodeRadius;
    const startY = from.y + uy * nodeRadius;
    const endOffset = undirected ? nodeRadius : nodeRadius + 10;
    const endX = to.x - ux * endOffset;
    const endY = to.y - uy * endOffset;
    let pathD = "";
    let labelX = (startX + endX) / 2;
    let labelY = (startY + endY) / 2;
    let markerStart = "";
    let markerEnd = undirected ? "" : "url(#arrow-end)";
    let labelText = `${edge.id}:${edge.weight}`;

    if (edge.from === edge.to) {
      const loopR = 25;
      pathD = `M ${from.x} ${from.y - nodeRadius} C ${from.x + loopR} ${from.y - 45}, ${from.x - loopR} ${from.y - 45}, ${from.x} ${from.y - nodeRadius}`;
      labelX = from.x;
      labelY = from.y - 50;
    } else if (hasReverse && !undirected) {
      if (edge.from > edge.to) return;
      usedPair.add(`${pairKeyBoth}-both`);
      pathD = `M ${startX} ${startY} L ${endX} ${endY}`;
      markerStart = "url(#arrow-start)";
      markerEnd = "url(#arrow-end)";
      labelX += px * 14;
      labelY += py * 14;
      labelText = `${edge.id}:${edge.weight} | ${reverseEdge.id}:${reverseEdge.weight}`;
    } else {
      pathD = `M ${startX} ${startY} L ${endX} ${endY}`;
      if (!undirected) {
        labelX += px * 14;
        labelY += py * 14;
      }
    }

    const pathAttrs = { d: pathD, fill: "none", stroke: "#2f3b52", "stroke-width": 2, "marker-end": markerEnd };
    if (markerStart) pathAttrs["marker-start"] = markerStart;
    const path = svgEl("path", pathAttrs);
    path.dataset.from = String(edge.from);
    path.dataset.to = String(edge.to);
    graphSvg.appendChild(path);

    const label = svgEl("text", {
      x: labelX,
      y: labelY,
      "text-anchor": "middle",
      "font-size": 16,
      fill: "#8f2d18",
      class: "edge-label"
    });
    label.textContent = labelText;
    graphSvg.appendChild(label);
  });

  nodes.forEach((node) => {
    const circle = svgEl("circle", {
      cx: node.x,
      cy: node.y,
      r: 20,
      fill: "#ffffff",
      stroke: "#1e66d0",
      "stroke-width": 2.5
    });
    circle.dataset.nodeId = String(node.id);
    circle.style.cursor = "pointer";
    circle.addEventListener("click", () => highlightNode(node.id));
    graphSvg.appendChild(circle);

    const text = svgEl("text", {
      x: node.x,
      y: node.y + 4,
      "text-anchor": "middle",
      "font-size": 14,
      fill: "#1e66d0",
      "font-weight": "700"
    });
    text.style.cursor = "pointer";
    text.textContent = String(node.id);
    text.addEventListener("click", () => highlightNode(node.id));
    graphSvg.appendChild(text);
  });
}

function highlightNode(nodeId) {
  const circles = graphSvg.querySelectorAll("circle");
  circles.forEach((c) => c.classList.toggle("node-active", Number(c.dataset.nodeId) === nodeId));

  const paths = graphSvg.querySelectorAll("path[data-from]");
  paths.forEach((p) => {
    const from = Number(p.dataset.from);
    const to = Number(p.dataset.to);
    p.classList.toggle("edge-active", from === nodeId || to === nodeId);
  });

  const adjCells = adjacencyMatrixContainer.querySelectorAll("td");
  adjCells.forEach((td) => {
    const r = Number(td.dataset.row);
    const c = Number(td.dataset.col);
    td.style.background = r === nodeId || c === nodeId ? "#fff4e6" : "";
  });
}

function renderMatrixTable(matrix, rowPrefix, colPrefix) {
  const table = document.createElement("table");
  const thead = document.createElement("thead");
  const headerRow = document.createElement("tr");
  const corner = document.createElement("th");
  corner.textContent = "";
  headerRow.appendChild(corner);
  for (let j = 0; j < matrix[0].length; j += 1) {
    const th = document.createElement("th");
    th.textContent = `${colPrefix}${j}`;
    headerRow.appendChild(th);
  }
  thead.appendChild(headerRow);
  table.appendChild(thead);

  const tbody = document.createElement("tbody");
  for (let i = 0; i < matrix.length; i += 1) {
    const row = document.createElement("tr");
    const rowHeader = document.createElement("th");
    rowHeader.textContent = `${rowPrefix}${i}`;
    row.appendChild(rowHeader);
    for (let j = 0; j < matrix[i].length; j += 1) {
      const td = document.createElement("td");
      td.textContent = String(matrix[i][j]);
      td.dataset.row = String(i);
      td.dataset.col = String(j);
      row.appendChild(td);
    }
    tbody.appendChild(row);
  }
  table.appendChild(tbody);
  return table;
}

function buildIncidenceMatrix(matrix) {
  const edges = buildDirectedEdges(matrix);
  const n = matrix.length;
  const incidence = Array.from({ length: n }, () => Array(edges.length).fill(0));
  edges.forEach((edge, edgeIdx) => {
    if (edge.from === edge.to) {
      incidence[edge.from][edgeIdx] = 2;
    } else {
      incidence[edge.from][edgeIdx] = -1;
      incidence[edge.to][edgeIdx] = 1;
    }
  });
  return { incidence };
}

function renderAdjacencyAndIncidence(matrix, undirected) {
  adjacencyMatrixContainer.innerHTML = "";
  incidenceMatrixContainer.innerHTML = "";
  adjacencyMatrixContainer.appendChild(renderMatrixTable(matrix, "v", "v"));
  if (undirected) {
    incidenceMatrixContainer.textContent = "树模式按无向边展示；关联矩阵按有向展开时可与上图邻接矩阵一致（对称）。";
    return;
  }
  const { incidence } = buildIncidenceMatrix(matrix);
  if (incidence.length === 0 || incidence[0].length === 0) {
    incidenceMatrixContainer.textContent = "当前图没有边，关联矩阵为空。";
    return;
  }
  incidenceMatrixContainer.appendChild(renderMatrixTable(incidence, "v", "e"));
}

function runWarshall(matrix) {
  const n = matrix.length;
  const closure = matrix.map((row) => row.map((v) => (v !== 0 ? 1 : 0)));
  for (let i = 0; i < n; i += 1) closure[i][i] = 1;
  for (let k = 0; k < n; k += 1) {
    for (let i = 0; i < n; i += 1) {
      for (let j = 0; j < n; j += 1) {
        closure[i][j] = closure[i][j] || (closure[i][k] && closure[k][j]) ? 1 : 0;
      }
    }
  }
  return closure;
}

function runDijkstra(start, matrix) {
  const n = matrix.length;
  const dist = Array(n).fill(Infinity);
  const visited = Array(n).fill(false);
  dist[start] = 0;
  for (let cnt = 0; cnt < n; cnt += 1) {
    let curr = -1;
    let minVal = Infinity;
    for (let i = 0; i < n; i += 1) {
      if (!visited[i] && dist[i] < minVal) {
        minVal = dist[i];
        curr = i;
      }
    }
    if (curr === -1) break;
    visited[curr] = true;
    for (let nxt = 0; nxt < n; nxt += 1) {
      if (matrix[curr][nxt] > 0 && !visited[nxt]) {
        const nd = dist[curr] + matrix[curr][nxt];
        if (nd < dist[nxt]) dist[nxt] = nd;
      }
    }
  }
  return dist.map((d) => (d === Infinity ? "INF" : d));
}

function runHamilton(matrix) {
  const n = matrix.length;
  const visited = Array(n).fill(false);
  const routes = [];

  function dfs(curr, route) {
    if (route.length === n) {
      routes.push([...route]);
      return;
    }
    for (let nxt = 0; nxt < n; nxt += 1) {
      if (matrix[curr][nxt] !== 0 && !visited[nxt]) {
        visited[nxt] = true;
        route.push(nxt);
        dfs(nxt, route);
        route.pop();
        visited[nxt] = false;
      }
    }
  }

  visited[0] = true;
  dfs(0, [0]);
  return routes;
}

function runKruskalUndirected(undAdj) {
  const n = undAdj.length;
  if (n <= 1) return { connected: true, total: 0, edges: [] };
  const edges = [];
  for (let i = 0; i < n; i += 1) {
    for (let j = i + 1; j < n; j += 1) {
      const w = undAdj[i][j];
      if (w === KRUSKAL_NO_EDGE) continue;
      edges.push({ u: i, v: j, w });
    }
  }
  edges.sort((a, b) => a.w - b.w);
  const p = Array.from({ length: n }, (_, i) => i);
  function find(x) {
    return p[x] === x ? x : (p[x] = find(p[x]));
  }
  function unite(a, b) {
    a = find(a);
    b = find(b);
    if (a === b) return false;
    p[a] = b;
    return true;
  }
  const mst = [];
  let tot = 0;
  for (const e of edges) {
    if (unite(e.u, e.v)) {
      mst.push(e);
      tot += e.w;
      if (mst.length === n - 1) break;
    }
  }
  const connected = mst.length === n - 1;
  return { connected, total: connected ? tot : 0, edges: connected ? mst : [] };
}

function runFord(start, matrix) {
  const n = matrix.length;
  const INF = Number.MAX_SAFE_INTEGER / 4;
  const dist = Array(n).fill(INF);
  if (start < 0 || start >= n) return { dist, neg: false };
  dist[start] = 0;
  for (let it = 0; it < n - 1; it += 1) {
    let upd = false;
    for (let u = 0; u < n; u += 1) {
      if (dist[u] >= INF) continue;
      for (let v = 0; v < n; v += 1) {
        if (matrix[u][v] === 0) continue;
        const nd = dist[u] + matrix[u][v];
        if (nd < dist[v]) {
          dist[v] = nd;
          upd = true;
        }
      }
    }
    if (!upd) break;
  }
  for (let u = 0; u < n; u += 1) {
    if (dist[u] >= INF) continue;
    for (let v = 0; v < n; v += 1) {
      if (matrix[u][v] === 0) continue;
      if (dist[u] + matrix[u][v] < dist[v]) {
        return { dist, neg: true };
      }
    }
  }
  return { dist, neg: false };
}

function runTsp(start, matrix) {
  const n = matrix.length;
  let bestCost = Infinity;
  let bestPath = null;
  const path = [start];
  const vis = Array(n).fill(false);
  vis[start] = true;

  function dfs(u, cost, depth) {
    if (depth === n) {
      const back = matrix[u][start];
      if (back !== 0) {
        const c = cost + back;
        if (c < bestCost) {
          bestCost = c;
          bestPath = [...path, start];
        }
      }
      return;
    }
    for (let v = 0; v < n; v += 1) {
      if (vis[v]) continue;
      const w = matrix[u][v];
      if (w === 0) continue;
      vis[v] = true;
      path.push(v);
      dfs(v, cost + w, depth + 1);
      path.pop();
      vis[v] = false;
    }
  }

  dfs(start, 0, 1);
  return { path: bestPath, cost: bestCost };
}

/** Bareiss 算法，整数精确行列式 */
function bareissDeterminant(mat0) {
  const n = mat0.length;
  if (n === 0) return 1;
  const A = mat0.map((r) => r.map((x) => x));
  let sign = 1;
  let prev = 1;
  for (let k = 0; k < n - 1; k += 1) {
    if (A[k][k] === 0) {
      let s = k + 1;
      while (s < n && A[s][k] === 0) s += 1;
      if (s === n) return 0;
      const tmp = A[k];
      A[k] = A[s];
      A[s] = tmp;
      sign *= -1;
    }
    for (let i = k + 1; i < n; i += 1) {
      for (let j = k + 1; j < n; j += 1) {
        A[i][j] = (A[i][j] * A[k][k] - A[i][k] * A[k][j]) / prev;
      }
    }
    prev = A[k][k];
  }
  return sign * A[n - 1][n - 1];
}

/** Kirchhoff：无向简单图生成树数目 */
function spanningTreeCountUndirected(adj01) {
  const n = adj01.length;
  if (n <= 1) return 1;
  const L = Array.from({ length: n }, () => Array(n).fill(0));
  for (let i = 0; i < n; i += 1) {
    for (let j = 0; j < n; j += 1) {
      if (i !== j && adj01[i][j]) {
        L[i][j] = -1;
        L[i][i] += 1;
      }
    }
  }
  const sub = Array.from({ length: n - 1 }, () => Array(n - 1).fill(0));
  for (let i = 0; i < n - 1; i += 1) {
    for (let j = 0; j < n - 1; j += 1) sub[i][j] = L[i][j];
  }
  return bareissDeterminant(sub);
}

function runHuffmanReport(leafWeights) {
  let w = leafWeights.slice();
  const origN = w.length;
  if (origN < 2) return "Huffman：至少需要 2 个叶权（可对角线读取）。\n";
  if (w.some((x) => x < 0 || !Number.isInteger(x))) return "Huffman：叶权须为非负整数。\n";
  if (w.reduce((a, b) => a + b, 0) === 0) return "Huffman：权值和为 0，无法建树。\n";
  let m = w.length;
  while (m > 1 && m % 2 !== 0) {
    w.push(0);
    m += 1;
  }
  const nodes = [];
  const pq = [];
  const pushPq = (ww, idx) => {
    pq.push({ ww, idx });
    pq.sort((a, b) => (a.ww !== b.ww ? a.ww - b.ww : a.idx - b.idx));
  };
  const popPq = () => pq.shift();
  for (let i = 0; i < w.length; i += 1) {
    nodes.push({ weight: w[i], left: -1, right: -1, leaf: i });
    pushPq(w[i], nodes.length - 1);
  }
  let out = "Huffman（合并次序）：\n";
  let step = 1;
  while (pq.length >= 2) {
    const a = popPq();
    const b = popPq();
    const na = nodes[a.idx];
    const nb = nodes[b.idx];
    const nw = na.weight + nb.weight;
    nodes.push({ weight: nw, left: a.idx, right: b.idx, leaf: -1 });
    const pidx = nodes.length - 1;
    out += `  步骤 ${step++}：合并权 ${na.weight} 与 ${nb.weight} -> 新结点权 ${nw}\n`;
    pushPq(nw, pidx);
  }
  const root = pq.length ? pq[0].idx : 0;
  const codes = Array(w.length).fill("");
  function gen(idx, pre) {
    const nd = nodes[idx];
    if (nd.left < 0 && nd.right < 0) {
      if (nd.leaf >= 0 && nd.leaf < codes.length) codes[nd.leaf] = pre || "0";
      return;
    }
    if (nd.left >= 0) gen(nd.left, pre + "0");
    if (nd.right >= 0) gen(nd.right, pre + "1");
  }
  gen(root, "");
  out += "叶结点编码（原始索引，权，码字）：\n";
  for (let i = 0; i < origN; i += 1) {
    out += `  叶 ${i} 权=${leafWeights[i]} 码=${codes[i]}\n`;
  }
  out += "说明：已用 0 权虚叶补足使可反复两两合并。\n";
  return out;
}

function updateStartNodeVisibility() {
  const mode = algorithmSelect.value;
  const show = currentInputKind === "graph" && (mode === "dijkstra" || mode === "ford" || mode === "tsp");
  startNodeLabel.classList.toggle("hidden", !show);
  startNodeInput.classList.toggle("hidden", !show);
}

inputKindRadios.forEach((r) => {
  r.addEventListener("change", () => {
    updateInputHelp();
    currentInputKind = getInputKind();
    if (matrixEditorSize > 0) {
      const preset = currentMatrix.length === matrixEditorSize ? currentMatrix : null;
      createMatrixEditor(matrixEditorSize, preset);
    }
    rebuildAlgorithmOptions();
  });
});

generateTableBtn.addEventListener("click", () => {
  inputError.textContent = "";
  const n = Number(nodeCountInput.value);
  if (Number.isNaN(n) || !Number.isInteger(n) || n <= 0 || n > 30) {
    inputError.textContent = "节点个数请输入 1 到 30 的整数。";
    return;
  }
  currentInputKind = getInputKind();
  const preset = currentMatrix.length === n ? currentMatrix : null;
  createMatrixEditor(n, preset);
});

buildGraphBtn.addEventListener("click", () => {
  inputError.textContent = "";
  algorithmError.textContent = "";
  algoOutput.textContent = "";
  currentInputKind = getInputKind();
  try {
    currentMatrix = readMatrixFromEditor();
  } catch (err) {
    inputError.textContent = err.message;
    return;
  }
  currentUndirected = currentInputKind === "tree";
  if (currentInputKind === "graph") {
    currentHasNegative = matrixHasNegativeEdge(currentMatrix);
  } else {
    currentHasNegative = false;
  }
  rebuildAlgorithmOptions();

  if (currentUndirected) drawGraph(currentMatrix, true);
  else drawGraph(currentMatrix, false);

  renderAdjacencyAndIncidence(currentMatrix, currentUndirected);
  algorithmSection.classList.remove("hidden");
  resultSection.classList.remove("hidden");
  algoOutputSection.classList.remove("hidden");
  startNodeInput.max = String(currentMatrix.length - 1);
  if (Number(startNodeInput.value) >= currentMatrix.length) startNodeInput.value = "0";
  const label = currentUndirected ? "树" : "图";
  algoOutput.textContent = `${label}已生成。请选择算法并点击「运行算法」。点击节点可高亮。`;
});

algorithmSelect.addEventListener("change", updateStartNodeVisibility);
runAlgorithmBtn.addEventListener("click", () => {
  if (!currentMatrix.length) {
    algorithmError.textContent = "请先输入并生成图。";
    return;
  }
  algorithmError.textContent = "";
  const mode = algorithmSelect.value;
  if (mode === "none") {
    algoOutput.textContent = currentUndirected ? "当前仅展示树结构。" : "当前仅展示图结构。";
    return;
  }

  if (currentInputKind === "tree") {
    if (mode === "tree_count") {
      const n = currentMatrix.length;
      const adj01 = Array.from({ length: n }, () => Array(n).fill(0));
      for (let i = 0; i < n; i += 1) {
        for (let j = 0; j < n; j += 1) {
          if (i !== j) adj01[i][j] = currentMatrix[i][j] > 0 ? 1 : 0;
        }
      }
      const cnt = spanningTreeCountUndirected(adj01);
      algoOutput.textContent = `生成树数量（矩阵树定理，无向拓扑）：${cnt}\n（本输入已是一棵树，故通常为 1。）`;
      return;
    }
    if (mode === "kruskal") {
      const res = runKruskalUndirected(symTreeToKruskalUndirected(currentMatrix));
      if (!res.connected) {
        algoOutput.textContent = "Kruskal：不连通";
        return;
      }
      algoOutput.textContent = `Kruskal MST 总权：${res.total}\n边：\n${res.edges.map((e) => `  (${e.u},${e.v}) 权 ${e.w}`).join("\n")}`;
      return;
    }
    if (mode === "huffman") {
      const diag = currentMatrix.map((row, i) => row[i]);
      algoOutput.textContent = runHuffmanReport(diag);
      return;
    }
  }

  /* graph模式 */
  if (currentHasNegative && mode !== "ford") {
    algorithmError.textContent = "检测到负权边：只能使用 Ford / Bellman-Ford。";
    return;
  }

  if (mode === "warshall") {
    const closure = runWarshall(currentMatrix);
    algoOutput.textContent = `Warshall 可达闭包（非零视为有边）：\n${closure.map((r) => r.join(" ")).join("\n")}`;
    return;
  }
  if (mode === "dijkstra") {
    const start = Number(startNodeInput.value);
    if (Number.isNaN(start) || start < 0 || start >= currentMatrix.length || !Number.isInteger(start)) {
      algorithmError.textContent = `起点必须是 0 到 ${currentMatrix.length - 1} 之间的整数。`;
      return;
    }
    const dist = runDijkstra(start, currentMatrix);
    algoOutput.textContent = `Dijkstra 最短路（起点 v${start}，仅正权边松弛）：\n${dist.map((d, i) => `v${i}: ${d}`).join("\n")}`;
    return;
  }
  if (mode === "hamilton") {
    const routes = runHamilton(currentMatrix);
    if (routes.length === 0) {
      algoOutput.textContent = "Hamilton：未找到从 v0 出发覆盖全部顶点的路径（非零视为有边）。";
      return;
    }
    algoOutput.textContent = `Hamilton 结果（共 ${routes.length} 条）：\n${routes.map((r, idx) => `${idx + 1}. ${r.map((v) => `v${v}`).join(" -> ")}`).join("\n")}`;
    return;
  }
  if (mode === "kruskal") {
    const und = toUndirectedForMst(currentMatrix);
    const res = runKruskalUndirected(und);
    if (!res.connected) {
      algoOutput.textContent = "Kruskal：无向化后的图不连通";
      return;
    }
    algoOutput.textContent = `Kruskal（无向化：双边取较小权，单边保留）MST 总权：${res.total}\n边：\n${res.edges.map((e) => `  (${e.u},${e.v}) 权 ${e.w}`).join("\n")}`;
    return;
  }
  if (mode === "tsp") {
    const start = Number(startNodeInput.value);
    if (Number.isNaN(start) || start < 0 || start >= currentMatrix.length || !Number.isInteger(start)) {
      algorithmError.textContent = `起点必须是 0 到 ${currentMatrix.length - 1} 之间的整数。`;
      return;
    }
    const { path: p, cost: c } = runTsp(start, currentMatrix);
    if (!p || c === Infinity) {
      algoOutput.textContent = `旅行商（起点 v${start}）：不存在经过所有顶点并回到起点的有向回路（0 表示无边）。`;
      return;
    }
    algoOutput.textContent = `旅行商 TSP（DFS）：最小总权 ${c}\n最优回路：${p.map((v) => `v${v}`).join(" -> ")}`;
    return;
  }
  if (mode === "ford") {
    const start = Number(startNodeInput.value);
    if (Number.isNaN(start) || start < 0 || start >= currentMatrix.length || !Number.isInteger(start)) {
      algorithmError.textContent = `起点必须是 0 到 ${currentMatrix.length - 1} 之间的整数。`;
      return;
    }
    const { dist, neg } = runFord(start, currentMatrix);
    if (neg) {
      algoOutput.textContent = "Ford：检测到从起点可达的负权回路。";
      return;
    }
    const lines = dist.map((d, i) => `v${i}: ${d >= Number.MAX_SAFE_INTEGER / 8 ? "INF" : d}`);
    algoOutput.textContent = `Ford / Bellman-Ford（起点 v${start}，0 表示无边，否则为有向边权可为负）：\n${lines.join("\n")}`;
  }
});

updateInputHelp();
currentInputKind = getInputKind();
createMatrixEditor(4, [
  [0, 5, 0, 2],
  [0, 0, 3, 0],
  [1, 0, 0, 4],
  [0, 0, 0, 0]
]);
rebuildAlgorithmOptions();
updateStartNodeVisibility();
