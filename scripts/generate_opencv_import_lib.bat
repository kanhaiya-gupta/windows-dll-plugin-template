@echo off
REM Generate OpenCV import libraries (.lib) from the DLLs.
REM Run from "x64 Native Tools Command Prompt for VS 2022".
REM From repo root: scripts\generate_opencv_import_lib.bat
REM Or use PowerShell: .\scripts\generate_opencv_import_lib.ps1

powershell -ExecutionPolicy Bypass -File "%~dp0generate_opencv_import_lib.ps1"
exit /b %ERRORLEVEL%
