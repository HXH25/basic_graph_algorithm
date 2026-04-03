@echo off
setlocal
cd /d "%~dp0"

echo [offline] Starting local server...
python run_offline.py --host 127.0.0.1 --port 8000

endlocal
