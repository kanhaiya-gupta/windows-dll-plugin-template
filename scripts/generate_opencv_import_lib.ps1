# Generate OpenCV import libraries (.lib) from the DLLs.
# The official OpenCV Windows pack often ships only DLLs. This script creates
# the .lib files so the linker can find them.
# Run from any PowerShell; the script finds Visual Studio tools automatically.
# From repo root: .\scripts\generate_opencv_import_lib.ps1

$ErrorActionPreference = "Stop"

# Find dumpbin and lib (Visual Studio VC tools)
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $dumpbinPath = & $vswhere -latest -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find "VC/Tools/MSVC/*/bin/Hostx64/x64/dumpbin.exe" 2>$null | Select-Object -First 1
    if ($dumpbinPath) {
        $vcBin = Split-Path -Parent $dumpbinPath
        $env:PATH = "$vcBin;$env:PATH"
    }
}
if (-not (Get-Command dumpbin -ErrorAction SilentlyContinue)) {
    Write-Host "dumpbin not found. Open 'x64 Native Tools Command Prompt for VS 2022' and run this script again, or install Visual Studio C++ tools."
    exit 1
}

$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$RepoRoot = (Resolve-Path (Join-Path $ScriptDir "..")).Path
$OpenCVDir = Join-Path $RepoRoot "third_party\opencv"
$OutLibDir = Join-Path $OpenCVDir "build\x64\generated_lib"
$DllDirs = @(
    (Join-Path $OpenCVDir "build\x64\vc16\bin"),
    (Join-Path $OpenCVDir "build\x64\vc15\bin"),
    (Join-Path $OpenCVDir "build\x64\bin")
)

function Find-Dll($name) {
    foreach ($d in $DllDirs) {
        $p = Join-Path $d $name
        if (Test-Path $p) { return $p }
    }
    return $null
}

function Export-DefFromDll($dllPath, $defPath) {
    $dllName = [System.IO.Path]::GetFileName($dllPath)
    $exports = & dumpbin /EXPORTS $dllPath 2>&1
    $out = New-Object System.Collections.Generic.List[string]
    $out.Add("LIBRARY $dllName")
    $out.Add("EXPORTS")
    $inTable = $false
    foreach ($line in $exports) {
        # Dumpbin: "  ordinal  hint   RVA     name" - split by whitespace, 4th token is symbol (rest may be demangled text)
        $parts = $line.Trim() -split '\s+', 4
        if ($parts.Count -ge 4 -and $parts[0] -match '^\d+$' -and $parts[2] -match '^[0-9A-Fa-f]+$') {
            $name = ($parts[3] -split '\s+')[0]
            if ($name.Length -gt 1 -and $name -notmatch '[(),]') {
                $out.Add("    $name")
            }
        }
    }
    [System.IO.File]::WriteAllLines($defPath, $out)
}

$dllD = Find-Dll "opencv_world410d.dll"
$dllR = Find-Dll "opencv_world410.dll"
if (-not $dllD) { $dllD = Find-Dll "opencv_world4100d.dll" }
if (-not $dllR) { $dllR = Find-Dll "opencv_world4100.dll" }

if (-not $dllD -or -not $dllR) {
    Write-Host "Could not find OpenCV DLLs in:"
    foreach ($d in $DllDirs) { Write-Host "  $d" }
    Write-Host "Ensure OpenCV is extracted to third_party\opencv with build\x64\...\bin containing opencv_world*.dll"
    exit 1
}

New-Item -ItemType Directory -Force -Path $OutLibDir | Out-Null
$defD = Join-Path $OutLibDir "opencv_world410d.def"
$defR = Join-Path $OutLibDir "opencv_world410.def"
$libD = Join-Path $OutLibDir "opencv_world410d.lib"
$libR = Join-Path $OutLibDir "opencv_world410.lib"

Write-Host "Creating import lib for Debug DLL..."
Export-DefFromDll $dllD $defD
& lib /DEF:$defD /OUT:$libD /MACHINE:X64
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Creating import lib for Release DLL..."
Export-DefFromDll $dllR $defR
& lib /DEF:$defR /OUT:$libR /MACHINE:X64
if ($LASTEXITCODE -ne 0) { exit 1 }

Write-Host "Done. Import libraries written to: $OutLibDir"
Write-Host "  $libD"
Write-Host "  $libR"
Write-Host "Rebuild your solution."
