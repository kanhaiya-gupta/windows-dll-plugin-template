#!/usr/bin/env bash
# Download and extract OpenCV Windows prebuilt to third_party/opencv.
# Run from repo root: ./scripts/download_opencv.sh
# Or: bash scripts/download_opencv.sh
# Requires: curl (or wget). On Windows use Git Bash or WSL.

set -e

OPENCV_VERSION="4.10.0"
OPENCV_EXE="opencv-${OPENCV_VERSION}-windows.exe"
DOWNLOAD_URL="https://sourceforge.net/projects/opencvlibrary/files/${OPENCV_VERSION}/${OPENCV_EXE}/download"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
TARGET_DIR="$REPO_ROOT/third_party/opencv"
OPENCV_H="$TARGET_DIR/build/include/opencv2/opencv.hpp"

if [ -f "$OPENCV_H" ]; then
    echo "OpenCV already present at $TARGET_DIR. Skipping download."
    exit 0
fi

echo "Downloading OpenCV ${OPENCV_VERSION} Windows pack..."
echo "URL: $DOWNLOAD_URL"
echo "Target: $TARGET_DIR"
mkdir -p "$TARGET_DIR"

TMP_EXE="${TMPDIR:-/tmp}/${OPENCV_EXE}"
if [ -z "$TMPDIR" ] && [ -n "$USERPROFILE" ]; then
    TMP_EXE="$USERPROFILE/AppData/Local/Temp/${OPENCV_EXE}"
fi

if command -v curl &>/dev/null; then
    curl -L -o "$TMP_EXE" "$DOWNLOAD_URL"
elif command -v wget &>/dev/null; then
    wget -O "$TMP_EXE" "$DOWNLOAD_URL"
else
    echo "Need curl or wget to download. Install one and re-run."
    exit 1
fi

echo "Extracting to $TARGET_DIR ..."
if command -v cygpath &>/dev/null; then
    TARGET_WIN="$(cygpath -w "$TARGET_DIR")"
    TMP_EXE_WIN="$(cygpath -w "$TMP_EXE")"
else
    TARGET_WIN="$TARGET_DIR"
    TMP_EXE_WIN="$TMP_EXE"
fi

chmod +x "$TMP_EXE"
# Use a .bat file to avoid cmd.exe quoting issues with paths
BAT_DIR="$(dirname "$TMP_EXE")"
BAT_PATH="$BAT_DIR/run_opencv_install.bat"
printf '"%s" /D="%s"\r\n' "$TMP_EXE_WIN" "$TARGET_WIN" > "$BAT_PATH"

if command -v cygpath &>/dev/null; then
    BAT_WIN="$(cygpath -w "$BAT_PATH")"
else
    BAT_WIN="$BAT_PATH"
fi

if [[ "$OSTYPE" == msys ]] || [[ "$OSTYPE" == cygwin ]] || [[ "$OSTYPE" == win32 ]]; then
    cmd //c "$BAT_WIN"
else
    if command -v cmd.exe &>/dev/null; then
        cmd.exe //c "$BAT_WIN"
    else
        echo "Downloaded to: $TMP_EXE"
        echo "On Windows, run it and choose install path: $TARGET_DIR"
        exit 1
    fi
fi
rm -f "$BAT_PATH"

# Installer may create TargetDir/opencv/build instead of TargetDir/build
if [ ! -f "$OPENCV_H" ] && [ -d "$TARGET_DIR/opencv/build" ]; then
    mv "$TARGET_DIR/opencv/"* "$TARGET_DIR/"
    rmdir "$TARGET_DIR/opencv" 2>/dev/null || true
fi

if [ ! -f "$OPENCV_H" ]; then
    echo "After extract, expected: $OPENCV_H"
    echo "If missing, run the installer manually and select: $TARGET_DIR"
    exit 1
fi

echo "OpenCV installed to $TARGET_DIR. You can delete $TMP_EXE to save space."
echo "Done."
