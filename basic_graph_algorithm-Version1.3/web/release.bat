@echo off
setlocal
cd /d "%~dp0"

set "DIST_DIR=dist"
set "PKG_NAME=graph_visualization_offline"
set "STAGE_DIR=%DIST_DIR%\%PKG_NAME%"
set "ZIP_PATH=%DIST_DIR%\%PKG_NAME%.zip"

echo [release] preparing folders...
if not exist "%DIST_DIR%" mkdir "%DIST_DIR%"
if exist "%STAGE_DIR%" rmdir /s /q "%STAGE_DIR%"
mkdir "%STAGE_DIR%"

echo [release] copying files...
copy /y "index.html" "%STAGE_DIR%\" >nul
copy /y "style.css" "%STAGE_DIR%\" >nul
copy /y "app.js" "%STAGE_DIR%\" >nul
if exist "background.jpg" copy /y "background.jpg" "%STAGE_DIR%\" >nul
copy /y "run_offline.py" "%STAGE_DIR%\" >nul
copy /y "start_offline.bat" "%STAGE_DIR%\" >nul
copy /y "README.md" "%STAGE_DIR%\" >nul
if exist "assets" xcopy /e /i /y "assets" "%STAGE_DIR%\assets\" >nul

echo [release] creating zip package...
if exist "%ZIP_PATH%" del /f /q "%ZIP_PATH%"
powershell -NoProfile -Command "Compress-Archive -Path '%STAGE_DIR%\*' -DestinationPath '%ZIP_PATH%' -Force"

if exist "%ZIP_PATH%" (
  echo [release] done: %ZIP_PATH%
) else (
  echo [release] failed to create zip.
  exit /b 1
)

echo [release] you can share this zip with others.
endlocal
