#!/usr/bin/env python3
"""
Offline launcher for graph visualization web app.
Starts a local static server and opens browser automatically.
"""

from __future__ import annotations

import argparse
import http.server
import socketserver
import threading
import time
import webbrowser
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run offline graph visualization app")
    parser.add_argument("--host", default="127.0.0.1", help="Bind host (default: 127.0.0.1)")
    parser.add_argument("--port", type=int, default=8000, help="Bind port (default: 8000)")
    parser.add_argument(
        "--no-browser",
        action="store_true",
        help="Do not open browser automatically",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    root = Path(__file__).resolve().parent

    handler = http.server.SimpleHTTPRequestHandler
    socketserver.TCPServer.allow_reuse_address = True

    with socketserver.TCPServer((args.host, args.port), handler) as server:
        url = f"http://{args.host}:{args.port}/"
        print(f"[offline] serving: {root}")
        print(f"[offline] url: {url}")
        print("[offline] press Ctrl+C to stop")

        # Serve from web directory.
        root_old = Path.cwd()
        try:
            # Change directory so SimpleHTTPRequestHandler serves local files.
            # Keep this explicit for predictable offline behavior.
            import os
            os.chdir(root)

            if not args.no_browser:
                threading.Thread(
                    target=lambda: (time.sleep(0.5), webbrowser.open(url)),
                    daemon=True,
                ).start()

            server.serve_forever()
        finally:
            import os
            os.chdir(root_old)


if __name__ == "__main__":
    main()
