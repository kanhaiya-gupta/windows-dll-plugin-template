# Third-party dependencies

This folder holds external libraries used by the MyDll1 plugin. Only the **MyDll1** project (DLL) references third_party; the public API in `include/` does not expose third-party types.

**Current third-party:** **OpenCV** — used in `MyDll1_GetThirdPartyCheck()` to verify the framework. The build requires OpenCV; see "OpenCV" below for setup.

---

## Problems and how we fixed them

When integrating OpenCV we hit the issues below. Use this as a reference if you see similar errors or add other third-party libraries that ship only DLLs.

| Problem | Cause | Fix |
|--------|--------|-----|
| **Linker: cannot open file 'opencv_world410d.lib'** | The official OpenCV Windows pack ships **only DLLs and headers**, no import libraries (`.lib`). The MSVC linker needs a `.lib` to resolve symbols. | Generate the import lib from the DLL. Run once: `scripts\generate_opencv_import_lib.bat` (or `.\scripts\generate_opencv_import_lib.ps1`). This creates `opencv_world410(d).lib` in `third_party\opencv\build\x64\generated_lib\`. The project is already set to use that folder first. |
| **Unresolved external symbol** (e.g. `cv::Mat::Mat`, `cv::Mat::zeros`) | The generated `.def` file used to create the `.lib` was missing C++ export names, or included invalid text. | The generate script was fixed to parse `dumpbin /EXPORTS` by columns and use only the **first token** as the export name (so demangled text like `(public: ...)` is not written into the `.def`). Re-run the script to regenerate the `.lib`. |
| **LNK1118: syntax error in '(struct'** (or similar in .def) | The `.def` contained demangled C++ text (e.g. `(struct ...)`) which is invalid in a .def file. | Same as above: the script writes only the real symbol name (e.g. `??0Mat@cv@@QEAA@XZ`) into the `.def`, not the rest of the line. |
| **'dumpbin' is not recognized** | `dumpbin` is only on PATH in a "Developer" or "x64 Native Tools" command prompt. | The generate script uses `vswhere` to find Visual Studio and adds the VC `bin` folder to PATH, so you can run it from **any** PowerShell (or via the `.bat`). |
| **Cannot find OpenCV headers / wrong path** | OpenCV was extracted elsewhere, or path has spaces/special chars. | Use **OpenCV.path.props**: copy `OpenCV.path.props.example` to `OpenCV.path.props` and set `OpenCVDir` to your OpenCV **root** (the folder that contains `build`). |
| **OpenCV installer didn't extract to the right folder** | The download script runs the `.exe` with `/D=...`; some installers ignore it. | Run the downloaded `.exe` manually and in the GUI choose the install path: `third_party\opencv` (full path e.g. `C:\...\MyDll1\third_party\opencv`). Or extract elsewhere and set `OpenCVDir` in OpenCV.path.props. |
| **"Not a valid Win32 application"** when loading the DLL | Architecture mismatch: 32-bit host loading 64-bit DLL (or vice versa). | This solution is **x64 only**. Build both the plugin and the test host for **x64** (Debug or Release). |

Full narrative and technical details: **docs/logs.md**.

---

## How to add a new third-party software

Follow these steps when adding another library (e.g. libFoo) to the plugin.

### 1. Decide where it lives

- **Preferred:** Put the library under `third_party/<name>/` (e.g. `third_party/libfoo/`) so the repo is self-contained.
- **Optional:** If it’s installed system-wide or elsewhere, use a **path props** file (like `OpenCV.path.props`) so the project can point to it without hardcoding paths.

### 2. Get headers and libraries

- **Headers:** The compiler needs the include path (e.g. `third_party/libfoo/include`).
- **Libraries:**  
  - If the pack provides **.lib** (import libraries): add the folder that contains `.lib` to **Additional Library Directories** and add the `.lib` name(s) to **Additional Dependencies**.  
  - If the pack provides **only .dll** (like the official OpenCV Windows pack): you must **generate** the import lib from the DLL (see "If the pack has only DLLs" below).

### 3. Wire the project (MyDll1.vcxproj)

- **C/C++ → General → Additional Include Directories:** Add the include path (e.g. `$(SolutionDir)third_party\libfoo\include` or `$(LibFooDir)\include` if using a props file).
- **Linker → General → Additional Library Directories:** Add the folder that contains the `.lib` file(s).
- **Linker → Input → Additional Dependencies:** Add the `.lib` name(s) (e.g. `libfoo.lib`; add `libfood.lib` for Debug if the library has a separate Debug lib).
- **Copy DLLs to output:** In **Build Events → Post-build**, add a command to copy the library’s DLL(s) to `$(OutDir)` so the plugin can load them at runtime (e.g. `xcopy /Y "path\to\libfoo.dll" "$(OutDir)"`).

### 4. If the pack has only DLLs (no .lib)

Many Windows binary packs ship only DLLs + headers. The linker still needs a `.lib` (import library) to resolve symbols.

1. **Generate the import lib from the DLL:**
   - Use **dumpbin /EXPORTS** &lt;dll&gt; to list exports.
   - Build a **.def** file with those export names (one per line; use the **mangled symbol** from dumpbin, not demangled text like `(public: ...)`).
   - Run **lib /DEF:your.def /OUT:your.lib /MACHINE:X64** to produce the `.lib`.
2. You can automate this with a script (see `scripts/generate_opencv_import_lib.ps1` for a full example). Important: parse dumpbin output by **columns** and use only the **first token** as the export name to avoid putting `(struct` or `(public:` into the .def (which causes LNK1118).
3. Put the generated `.lib` in a known folder (e.g. `third_party/libfoo/generated_lib/`) and add that folder to **Additional Library Directories** (first in the list so it’s preferred).

### 5. Optional: path override (like OpenCV.path.props)

If the library can be installed in different locations, the same idea as OpenCV works for any third-party: add a **&lt;Name&gt;.path.props.example** and use a property like **$(LibFooDir)** in the project so that library can be placed **anywhere** (different drive, shared SDK path, etc.).

1. Create **&lt;Name&gt;.path.props.example** in the solution folder with a single property, e.g. `<LibFooDir>$(SolutionDir)third_party\libfoo</LibFooDir>`.
2. In the vcxproj, use `$(LibFooDir)` for include path, library path, and DLL copy paths.
3. Document: "Copy to &lt;Name&gt;.path.props and set the path to your install."

### 6. Use in code only in src/

- Include the third-party headers only in **src/** (e.g. `plugin_impl.cpp`), not in public headers under `include/`. That way the plugin’s C API stays free of third-party types.
- Link the library only in the DLL project, not in the test host (unless the host needs it for its own code).

### 7. Document it here

- Add a short section under "Third-party" (e.g. "## LibFoo") with: where to get it, expected layout under `third_party/`, any path override, and whether a generate-import-lib step is needed.
- If you add a new problem/fix, add a row to the "Problems and how we fixed them" table above.

---

## OpenCV

### 1. Download (automatic)

From the repo root (Git Bash, WSL, or shell with `curl`/`wget`):

```bash
./scripts/download_opencv.sh
```

It downloads the OpenCV 4.10.0 Windows pack and tries to extract to `third_party/opencv`. If extraction fails, run the downloaded `.exe` manually and choose **`third_party/opencv`** as the install path.

### 2. Install here (manual)

- [OpenCV releases](https://opencv.org/releases/) → download the **Windows** pack (e.g. `opencv-4.x.x-windows.exe`).
- Run it and set install path to **`third_party/opencv`** (the folder that will contain `build`).

### 3. Layout (default)

Under **`third_party/opencv`** you should have:

```
third_party/opencv/
└── build/
    ├── include/
    │   └── opencv2/
    └── x64/
        ├── generated_lib/   ← created by generate script (opencv_world410(d).lib)
        └── vc15/            (or vc16)
            ├── bin/         (DLLs)
            └── lib/         (.lib if present in pack; often empty in official pack)
```

Required after setup:

- `third_party/opencv/build/include/opencv2/opencv.hpp`
- Either `build/x64/vc15/lib/opencv_world4xx(d).lib` **or** run the generate script to create `build/x64/generated_lib/opencv_world410(d).lib`
- `build/x64/vc15/bin/opencv_world4xx(d).dll`

### 4. If the pack has only DLLs (no .lib)

The official Windows pack often has **no .lib** files. Generate them once:

- **Command Prompt:** Open "x64 Native Tools Command Prompt for VS 2022", `cd` to repo root, run:  
  `scripts\generate_opencv_import_lib.bat`
- **PowerShell:** From repo root:  
  `.\scripts\generate_opencv_import_lib.ps1`

This creates `third_party\opencv\build\x64\generated_lib\opencv_world410d.lib` and `opencv_world410.lib`. Then rebuild the solution.

### 5. Version number (4xx)

The project is set for OpenCV **4.10** (`410`). For 4.11, 4.12, etc.:

1. Edit **MyDll1/MyDll1.vcxproj**: replace `410` with your version in **Additional Dependencies** and in the **post-build** copy commands.
2. If the script generates a different base name, adjust the script or the vcxproj so the linked lib and copied DLL names match.

If your pack uses **vc16** instead of **vc15**, replace `vc15` with `vc16` in the vcxproj (include path, library path, DLL copy path).

### 6. Override path (OpenCV not under `third_party/opencv`)

1. Copy **OpenCV.path.props.example** (solution folder) to **OpenCV.path.props**.
2. Set `OpenCVDir` to your OpenCV **root** (the folder that contains `build`), e.g.  
   `<OpenCVDir>C:/opencv</OpenCVDir>`.
3. Rebuild.

### 7. Use in code

Include OpenCV only in **src/** (e.g. `plugin_impl.cpp`), not in `include/MyDll1_API.h`:

```cpp
#include "pch.h"
#include "MyDll1_API.h"
#include <opencv2/opencv.hpp>
```

The post-build step copies the OpenCV DLLs to the DLL output directory.

---

**Quick checks:**

- `cannot open source file "opencv2/opencv.hpp"` → Set include path: put OpenCV under `third_party/opencv` or set **OpenCVDir** in OpenCV.path.props.
- `cannot open file 'opencv_world410d.lib'` → Run **scripts\generate_opencv_import_lib.bat** (or the .ps1 script), then rebuild.
