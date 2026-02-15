# Log: Plugin development with OpenCV as third-party

This document explains the problem we ran into when using OpenCV as third-party software in the PluginEngine plugin, and how we fixed it from top to bottom.

---

## 1. Project setup

- **Goal:** Build a Windows plugin DLL (PluginEngine) that uses **OpenCV** as a third-party library to verify the framework.
- **Layout:**
  - `include/` — headers only (`.h`). Public API in `PluginEngine_API.h`; no OpenCV types exposed here.
  - `src/` — implementation only (`.cpp`). OpenCV is used only in `src/` (e.g. `plugin_impl.cpp`).
  - `docs/` — documentation (build, architecture, API).
  - `tests/` — test host (PluginEngineTestHost) that loads the DLL and calls the API.
  - `third_party/` — external dependencies. OpenCV is installed or pointed to here.

The plugin exposes a C API (e.g. `PluginEngine_GetThirdPartyCheck()`) so the host does not depend on C++ or OpenCV. OpenCV is an implementation detail.

---

## 2. The problem (summary)

When building the plugin that uses OpenCV (C++ API, e.g. `cv::Mat::zeros`), we hit:

1. **Linker error:** `cannot open file 'opencv_world410d.lib'`
2. **Later:** `unresolved external symbol` for `cv::Mat::Mat()`, `cv::Mat::~Mat()`, `cv::Mat::zeros(...)`

**Root cause:** The official OpenCV Windows pack (from opencv.org / SourceForge) ships **only DLLs and headers**. It does **not** ship the **import libraries** (`.lib` files). On Windows, the linker needs a `.lib` to resolve symbols at link time (it then loads the actual code from the DLL at runtime). Without the `.lib`, the linker cannot resolve OpenCV symbols and reports “unresolved external symbol” or “cannot open file … .lib”.

So the problem is **not** that “you can’t use third-party (or C++) in a plugin.” The problem is that **this particular OpenCV package does not provide the import libraries** the MSVC linker expects.

---

## 3. Problem 1: “Cannot open file opencv_world410d.lib”

### What happened

- The project was set to link against `opencv_world410d.lib` (Debug) and `opencv_world410.lib` (Release), with library path pointing at `third_party\opencv\build\x64\vc15\lib` (and similar).
- The OpenCV pack had been extracted, but that pack **does not contain** `opencv_world410(d).lib` in any folder.
- Result: linker error **LNK1104** — “cannot open file 'opencv_world410d.lib'”.

### Fix 1a: Path and configuration

- **OpenCV.path.props** (optional): If OpenCV is not under `third_party/opencv`, copy `OpenCV.path.props.example` to `OpenCV.path.props` and set `OpenCVDir` to your OpenCV root (the folder that contains `build`). The project uses `$(OpenCVDir)` for include and lib paths.
- **Library search order:** The project was updated to look in several places: `build\x64\generated_lib`, `build\x64\vc15\lib`, `build\x64\vc16\lib`, `build\x64\lib`, so different OpenCV layouts (vc15/vc16 or flat) are supported.

### Fix 1b: Generating the missing .lib from the DLL

Because the official pack does **not** include `.lib` files, we **generate** them from the existing DLL using Microsoft tools:

1. **dumpbin /EXPORTS** &lt;DLL&gt; — lists all symbols exported by the DLL.
2. **.def file** — a text file listing those export names in the form expected by `lib.exe`.
3. **lib /DEF:… /OUT:… .lib /MACHINE:X64** — produces the import library (`.lib`) from the `.def`.

A script automates this so the pack “gets” the missing `.lib` files without changing the plugin design.

- **Script:** `scripts/generate_opencv_import_lib.ps1` (and `scripts/generate_opencv_import_lib.bat` which calls it).
- **Output:** `third_party\opencv\build\x64\generated_lib\opencv_world410d.lib` and `opencv_world410.lib`.
- **Project:** `PluginEngine.vcxproj` has `build\x64\generated_lib` as the first library directory, so the linker uses these generated `.lib` files.

**Steps for you:** Run the script once (e.g. from PowerShell at repo root: `.\scripts\generate_opencv_import_lib.ps1`), then rebuild. The “cannot open file … .lib” error goes away once the generated `.lib` files exist and the path is correct.

---

## 4. Problem 2: “Unresolved external symbol” (cv::Mat, etc.)

### What happened

After generating the import lib, the linker sometimes still reported:

- `unresolved external symbol "public: __cdecl cv::Mat::Mat(void)" (??0Mat@cv@@QEAA@XZ)`
- `unresolved external symbol "public: __cdecl cv::Mat::~Mat(void)" (??1Mat@cv@@QEAA@XZ)`
- `unresolved external symbol "public: static class cv::MatExpr __cdecl cv::Mat::zeros(int,int,int)" (?zeros@Mat@cv@@SA?AVMatExpr@2@HHH@Z)`

So the **generated** `.lib` did not contain these C++ symbols. That meant the **.def file** used to create the `.lib` was missing these export names.

### Why the .def was incomplete

The `.def` is built by parsing the output of `dumpbin /EXPORTS` on the OpenCV DLL. Two issues caused missing or bad entries:

1. **Line format:** Dumpbin often prints the export name and then extra text on the same line (e.g. demangled C++ text like `(public: ...)` or `(struct ...)`). The parser was either:
   - Requiring the line to end right after the symbol (so lines with trailing text were skipped), or
   - Taking the whole rest of the line as the “name,” which included `(struct` or similar and broke the `.def` syntax (e.g. **LNK1118: syntax error in '(struct' statement**).
2. **Which names to keep:** Initially only C++-mangled names (starting with `?`) were added; C-style exports (e.g. `cvCreateMat`) were dropped. For C++ API we need the mangled names (e.g. `??0Mat@cv@@QEAA@XZ`) in the `.def`.

### Fix 2: Robust export parsing in the script

- **Single token after RVA:** For each line of `dumpbin /EXPORTS`, we treat the export name as the **first token** after the RVA (ordinal, hint, RVA are the first three columns). Any demangled text or “(struct …)” after that token is **not** written into the `.def`. That way we both capture the real symbol and avoid `.def` syntax errors.
- **Column-based parsing:** The script parses by splitting the line into columns (ordinal, hint, RVA, rest), then takes the first word of the fourth column as the export name. This works regardless of how much extra text dumpbin appends.
- **Include all valid exports:** Both C++ mangled names (e.g. `??0Mat@cv@@QEAA@XZ`) and C-style names (e.g. `cvCreateMat`) are included. Names that would break the `.def` (e.g. containing `(`, `,`, `)`) are skipped.
- **VS tools:** The script uses `vswhere` to locate Visual Studio’s `dumpbin` and `lib`, so it works from a normal PowerShell prompt without opening “Developer Command Prompt.”

After these changes, the generated `.def` (and hence the generated `.lib`) contains the cv::Mat and related symbols, and the linker resolves them. The plugin can use the **OpenCV C++ API** (e.g. `cv::Mat::zeros`) as intended.

---

## 5. OpenCV download and extraction

### Issue

We wanted to avoid manual download. A script `scripts/download_opencv.sh` downloads the OpenCV Windows pack and should extract it to `third_party/opencv`.

- **Extraction from script:** The installer `.exe` was often run with `/D=...` from a shell; quoting and path handling (e.g. in Git Bash / cmd) caused “not recognized as internal or external command” or the installer ignored the path and extracted elsewhere.
- **Result:** `third_party/opencv` was sometimes empty after the script, or the user had to run the installer manually.

### Fix

- **Script:** Uses a temporary `.bat` file with the exact `"path\to\opencv.exe" /D="path\to\third_party\opencv"` so cmd runs the installer with the correct target folder. Paths are converted with `cygpath -w` when running from Git Bash.
- **Manual fallback:** If the script does not extract correctly, run the downloaded `.exe` by hand and in the GUI set the extraction path to `third_party\opencv` (full path, e.g. `C:\...\PluginEngine\third_party\opencv`).
- **Path override:** If OpenCV ends up somewhere else (e.g. `C:\opencv`), use **OpenCV.path.props** and set `OpenCVDir` to that folder so the project still finds headers and DLLs.

Documented in `third_party/README.md`.

---

## 6. Platform (x64 only)

### Issue

Running the test host sometimes produced “not a valid application (architecture mismatch).” That usually means the host and the DLL were built for different architectures (e.g. 32-bit host loading 64-bit DLL).

### Fix

- The solution and projects were restricted to **x64 only** (no 32-bit platform) so the DLL and test host always match.
- The test host prints a clear message if LoadLibrary fails with error 193 (ERROR_BAD_EXE_FORMAT), telling the user to build both for the same platform.

---

## 7. Summary: problem and fix

| Problem | Cause | Fix |
|--------|--------|-----|
| Cannot open file `opencv_world410d.lib` | Official OpenCV Windows pack has no `.lib` files | Generate `.lib` from the DLL with `scripts/generate_opencv_import_lib.ps1`; project points to `build\x64\generated_lib`. |
| Unresolved external symbol (cv::Mat, zeros, etc.) | Generated `.def` missed C++ exports or included invalid text | Parse `dumpbin /EXPORTS` by columns; take only the first token as export name; include all valid C/C++ names; skip names that break `.def`. |
| LNK1118 syntax error in '(struct' | Export “name” in `.def` included demangled text like `(struct ...)` | Export name in `.def` is only the first token after RVA (the real symbol); rest of line ignored. |
| dumpbin not recognized | Script run outside Developer environment | Script uses vswhere to find VS and adds the VC `bin` path so `dumpbin` and `lib` are found. |
| OpenCV path / extraction | Path or installer behavior | OpenCV.path.props for custom path; download script + manual extract fallback; docs in `third_party/README.md`. |
| “Not a valid application (architecture mismatch)” | Architecture mismatch (x86 vs x64) | Solution is x64-only; test host and DLL both built for x64. |

---

## 8. Can we use C++ third-party in a plugin?

**Yes.** The plugin uses the OpenCV **C++ API** (e.g. `cv::Mat`, `cv::Mat::zeros`). The only blocker was the **missing import libraries** in the official pack. Once we generate the `.lib` from the DLL and parse exports correctly, the linker resolves all C++ symbols and the plugin builds and runs with OpenCV as a C++ third-party dependency.

---

## 9. Files and scripts reference

- **docs/README.md** — Build and usage.
- **docs/ARCHITECTURE.md** — Layout (include, src, docs, tests).
- **docs/API.md** — Plugin C API.
- **third_party/README.md** — OpenCV install, layout, path override, and “if pack has only DLLs” (run generate script).
- **scripts/download_opencv.sh** — Download OpenCV Windows pack; extract to `third_party/opencv` (or instruct user).
- **scripts/generate_opencv_import_lib.ps1** — Generate `opencv_world410(d).lib` from the DLL (dumpbin + .def + lib). Call from PowerShell or via `scripts/generate_opencv_import_lib.bat`.
- **OpenCV.path.props.example** — Template for custom OpenCV root; copy to `OpenCV.path.props` and set `OpenCVDir`.

End of log.
