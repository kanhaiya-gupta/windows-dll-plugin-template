# Download and extract OpenCV Windows prebuilt to third_party/opencv.
# Run from repo root: .\scripts\download_opencv.ps1
# Or from scripts: .\download_opencv.ps1 (script detects repo root)

$ErrorActionPreference = "Stop"
$OPENCV_VERSION = "4.10.0"
$OPENCV_EXE = "opencv-$OPENCV_VERSION-windows.exe"
# SourceForge direct download (no redirect)
$DOWNLOAD_URL = "https://sourceforge.net/projects/opencvlibrary/files/$OPENCV_VERSION/$OPENCV_EXE/download"

$RepoRoot = $PSScriptRoot
if (Test-Path (Join-Path $PSScriptRoot "..\PluginEngine.sln")) {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}
$TargetDir = Join-Path $RepoRoot "third_party\opencv"
$TempExe = Join-Path $env:TEMP $OPENCV_EXE

if (Test-Path (Join-Path $TargetDir "build\include\opencv2\opencv.hpp")) {
    Write-Host "OpenCV already present at $TargetDir. Skipping download."
    exit 0
}

Write-Host "Downloading OpenCV $OPENCV_VERSION Windows pack..."
Write-Host "URL: $DOWNLOAD_URL"
Write-Host "Target: $TargetDir"
New-Item -ItemType Directory -Force -Path $TargetDir | Out-Null

try {
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $DOWNLOAD_URL -OutFile $TempExe -UseBasicParsing
} catch {
    Write-Host "Download failed. Try manually:"
    Write-Host "  1. Open https://opencv.org/releases/"
    Write-Host "  2. Download the Windows pack (e.g. $OPENCV_EXE)"
    Write-Host "  3. Run it and choose install path: $TargetDir"
    exit 1
}

Write-Host "Extracting to $TargetDir ..."
# OpenCV Windows .exe is often NSIS/Inno; try /D= for target directory
$proc = Start-Process -FilePath $TempExe -ArgumentList "/D=$TargetDir" -Wait -PassThru
if ($proc.ExitCode -ne 0) {
    # Some installers use /S for silent and still need /D=
    $proc2 = Start-Process -FilePath $TempExe -ArgumentList "/S", "/D=$TargetDir" -Wait -PassThru
    if ($proc2.ExitCode -ne 0) {
        Write-Host "Automatic extract failed (exit $($proc.ExitCode))."
        Write-Host "Please run the installer yourself: $TempExe"
        Write-Host "Choose install path: $TargetDir"
        exit 1
    }
}

# Installer may create TargetDir\opencv\build or TargetDir\build
$buildPath = Join-Path $TargetDir "build"
if (-not (Test-Path $buildPath)) {
    $altBuild = Join-Path $TargetDir "opencv\build"
    if (Test-Path $altBuild) {
        Move-Item (Join-Path $TargetDir "opencv\*") $TargetDir -Force
        Remove-Item (Join-Path $TargetDir "opencv") -Recurse -Force -ErrorAction SilentlyContinue
    }
}

if (-not (Test-Path (Join-Path $TargetDir "build\include\opencv2\opencv.hpp"))) {
    Write-Host "After extract, expected build\include\opencv2\opencv.hpp under $TargetDir"
    Write-Host "If missing, run the installer manually and select: $TargetDir"
    exit 1
}

Write-Host "OpenCV installed to $TargetDir. You can delete $TempExe to save space."
Write-Host "Done."
