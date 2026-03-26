const nodeCountInput = document.getElementById("nodeCountInput");
const generateTableBtn = document.getElementById("generateTableBtn");
const matrixEditorContainer = document.getElementById("matrixEditorContainer");
const buildGraphBtn = document.getElementById("buildGraphBtn");
const inputError = document.getElementById("inputError");

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

const tsinghuaLogo = document.getElementById("tsinghuaLogo");
const logoFallback = document.getElementById("logoFallback");

let currentMatrix = [];
let matrixEditorSize = 0;

function initLogo() {
  const candidates = [
    "./tsinghua_logo.png",
    "./qinghua.png",
    "./tsinghua.png",
    "./logo.png",
    "./校徽.png",
    "../qinghua.png",
    "../tsinghua.png",
    "../logo.png",
    "../校徽.png"
  ];
  let idx = 0;

  const tryNext = () => {
    if (idx >= candidates.length) {
      tsinghuaLogo.classList.add("hidden");
      logoFallback.classList.remove("hidden");
      return;
    }
    tsinghuaLogo.src = candidates[idx];
    idx += 1;
  };

  tsinghuaLogo.addEventListener("load", () => {
    logoFallback.classList.add("hidden");
    tsinghuaLogo.classList.remove("hidden");
  });
  tsinghuaLogo.addEventListener("error", tryNext);
  tryNext();
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
      input.min = "0";
      input.step = "1";
      input.className = "matrix-cell-input";
      input.dataset.row = String(i);
      input.dataset.col = String(j);
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
  const matrix = Array.from({ length: matrixEditorSize }, () => Array(matrixEditorSize).fill(0));
  const inputs = matrixEditorContainer.querySelectorAll(".matrix-cell-input");
  for (const input of inputs) {
    const row = Number(input.dataset.row);
    const col = Number(input.dataset.col);
    const val = Number(input.value);
    if (Number.isNaN(val) || val < 0 || !Number.isInteger(val)) {
      throw new Error(`v${row} -> v${col} 必须是非负整数。`);
    }
    matrix[row][col] = val;
  }
  return matrix;
}

function buildDirectedEdges(matrix) {
  const edges = [];
  for (let i = 0; i < matrix.length; i += 1) {
    for (let j = 0; j < matrix[i].length; j += 1) {
      const w = matrix[i][j];
      if (w > 0) {
        edges.push({ id: `e${edges.length}`, from: i, to: j, weight: w });
      }
    }
  }
  return edges;
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

function drawGraph(matrix) {
  clearSvg();
  createMarkers();

  const width = 760;
  const height = 520;
  const nodes = getNodePositions(matrix.length, width, height);
  const edges = buildDirectedEdges(matrix);
  const edgeMap = new Map(edges.map((e) => [`${e.from}-${e.to}`, e]));
  const usedPair = new Set();

  edges.forEach((edge) => {
    const reverseKey = `${edge.to}-${edge.from}`;
    const pairKey = `${Math.min(edge.from, edge.to)}-${Math.max(edge.from, edge.to)}`;
    if (edge.from !== edge.to && usedPair.has(`${pairKey}-both`)) return;

    const from = nodes[edge.from];
    const to = nodes[edge.to];
    const reverseEdge = edgeMap.get(reverseKey);
    const hasReverse = Boolean(reverseEdge);

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
    const endX = to.x - ux * (nodeRadius + 10);
    const endY = to.y - uy * (nodeRadius + 10);
    let pathD = "";
    let labelX = (startX + endX) / 2;
    let labelY = (startY + endY) / 2;
    let markerStart = "";
    let markerEnd = "url(#arrow-end)";
    let labelText = `${edge.id}:${edge.weight}`;

    if (edge.from === edge.to) {
      const loopR = 25;
      pathD = `M ${from.x} ${from.y - nodeRadius} C ${from.x + loopR} ${from.y - 45}, ${from.x - loopR} ${from.y - 45}, ${from.x} ${from.y - nodeRadius}`;
      labelX = from.x;
      labelY = from.y - 50;
    } else if (hasReverse) {
      if (edge.from > edge.to) return;
      usedPair.add(`${pairKey}-both`);
      pathD = `M ${startX} ${startY} L ${endX} ${endY}`;
      markerStart = "url(#arrow-start)";
      markerEnd = "url(#arrow-end)";
      labelX += px * 14;
      labelY += py * 14;
      labelText = `${edge.id}:${edge.weight} | ${reverseEdge.id}:${reverseEdge.weight}`;
    } else {
      pathD = `M ${startX} ${startY} L ${endX} ${endY}`;
      labelX += px * 14;
      labelY += py * 14;
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
    td.style.background = (r === nodeId || c === nodeId) ? "#fff4e6" : "";
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

function renderAdjacencyAndIncidence(matrix) {
  adjacencyMatrixContainer.innerHTML = "";
  incidenceMatrixContainer.innerHTML = "";
  adjacencyMatrixContainer.appendChild(renderMatrixTable(matrix, "v", "v"));
  const { incidence } = buildIncidenceMatrix(matrix);
  if (incidence.length === 0 || incidence[0].length === 0) {
    incidenceMatrixContainer.textContent = "当前图没有边，关联矩阵为空。";
    return;
  }
  incidenceMatrixContainer.appendChild(renderMatrixTable(incidence, "v", "e"));
}

function runWarshall(matrix) {
  const n = matrix.length;
  const closure = matrix.map((row) => row.map((v) => (v > 0 ? 1 : 0)));
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
      if (matrix[curr][nxt] > 0 && !visited[nxt]) {
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

function updateStartNodeVisibility() {
  const show = algorithmSelect.value === "dijkstra";
  startNodeLabel.classList.toggle("hidden", !show);
  startNodeInput.classList.toggle("hidden", !show);
}

generateTableBtn.addEventListener("click", () => {
  inputError.textContent = "";
  const n = Number(nodeCountInput.value);
  if (Number.isNaN(n) || !Number.isInteger(n) || n <= 0 || n > 30) {
    inputError.textContent = "节点个数请输入 1 到 30 的整数。";
    return;
  }
  const preset = currentMatrix.length === n ? currentMatrix : null;
  createMatrixEditor(n, preset);
});

buildGraphBtn.addEventListener("click", () => {
  inputError.textContent = "";
  algorithmError.textContent = "";
  algoOutput.textContent = "";
  try {
    currentMatrix = readMatrixFromEditor();
  } catch (err) {
    inputError.textContent = err.message;
    return;
  }
  drawGraph(currentMatrix);
  renderAdjacencyAndIncidence(currentMatrix);
  algorithmSection.classList.remove("hidden");
  resultSection.classList.remove("hidden");
  algoOutputSection.classList.remove("hidden");
  startNodeInput.max = String(currentMatrix.length - 1);
  if (Number(startNodeInput.value) >= currentMatrix.length) startNodeInput.value = "0";
  algoOutput.textContent = "图已生成。请选择算法并点击“运行算法”。点击图中的节点可高亮相关边和邻接矩阵。";
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
    algoOutput.textContent = "当前仅展示图结构。";
    return;
  }
  if (mode === "warshall") {
    const closure = runWarshall(currentMatrix);
    algoOutput.textContent = `Warshall 可达闭包:\n${closure.map((r) => r.join(" ")).join("\n")}`;
    return;
  }
  if (mode === "dijkstra") {
    const start = Number(startNodeInput.value);
    if (Number.isNaN(start) || start < 0 || start >= currentMatrix.length || !Number.isInteger(start)) {
      algorithmError.textContent = `起点必须是 0 到 ${currentMatrix.length - 1} 之间的整数。`;
      return;
    }
    const dist = runDijkstra(start, currentMatrix);
    algoOutput.textContent = `Dijkstra 最短路（起点 v${start}）:\n${dist.map((d, i) => `v${i}: ${d}`).join("\n")}`;
    return;
  }
  if (mode === "hamilton") {
    const routes = runHamilton(currentMatrix);
    if (routes.length === 0) {
      algoOutput.textContent = "Hamilton 结果：未找到从 v0 出发覆盖全部节点的路径。";
      return;
    }
    algoOutput.textContent = `Hamilton 结果（共 ${routes.length} 条）:\n${routes.map((r, idx) => `${idx + 1}. ${r.map((v) => `v${v}`).join(" -> ")}`).join("\n")}`;
  }
});

createMatrixEditor(4, [
  [0, 5, 0, 2],
  [0, 0, 3, 0],
  [1, 0, 0, 4],
  [0, 0, 0, 0]
]);
updateStartNodeVisibility();
initLogo();
