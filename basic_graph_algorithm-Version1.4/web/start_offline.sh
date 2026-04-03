#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"

PYTHON_BIN=""
if command -v python3 >/dev/null 2>&1; then
  PYTHON_BIN="python3"
elif command -v python >/dev/null 2>&1; then
  PYTHON_BIN="python"
else
  echo "[offline] Python is not installed. Please install Python 3 first." >&2
  exit 1
fi

echo "[offline] Starting local server..."
exec "${PYTHON_BIN}" run_offline.py --host 127.0.0.1 --port 8000 "$@"
