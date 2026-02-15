# PowerPoint and Excel plugins (ppt_excel branch)

This branch focuses on **Microsoft PowerPoint** and **Excel** plugins built on top of the native DLL template.

## How it fits

- **Native DLL (MyDll1)** — Stays as the **core engine**: C API, version, init/shutdown, third-party (e.g. OpenCV). No Office-specific code inside.
- **Office layer** — PowerPoint and Excel load a **COM add-in** (C# DLL that implements `IDTExtensibility2`). That add-in loads our native DLL and calls its C API.

```
PowerPoint / Excel  →  COM add-in (PowerPointAddIn / ExcelAddIn)  →  MyDll1Native (P/Invoke)  →  MyDll1.dll
```

## What was added

| Project | Role |
|--------|------|
| **MyDll1Native** | C# class library; P/Invoke wrapper for `MyDll1.dll` (GetVersion, Initialize, Shutdown, GetThirdPartyCheck). Used by both add-ins. |
| **PowerPointAddIn** | C# COM add-in for PowerPoint. Implements `IDTExtensibility2`: `OnConnection` → `MyDll1.Initialize()`, `OnDisconnection` → `MyDll1.Shutdown()`. Post-build copies `MyDll1.dll` and OpenCV DLLs to add-in output. |
| **ExcelAddIn** | Same for Excel. |

All three are in **addins/** and target **.NET Framework 4.8**, **x64** (to load the x64 native DLL).

## Build

### Visual Studio (recommended)

1. Open **MyDll1.sln** in Visual Studio 2022.
2. Select **Debug | x64** (or Release | x64).
3. **Build → Build Solution.**

The solution builds in order: **MyDll1** (native) → **MyDll1Native** → **PowerPointAddIn** / **ExcelAddIn**. The add-in projects depend on MyDll1 and MyDll1Native, so the native DLL is built first. A post-build step copies `MyDll1.dll` and the OpenCV DLLs from `MyDll1\x64\Debug` (or Release) into each add-in’s output folder.

### dotnet CLI (add-ins only)

You can build only the C# add-ins from the command line (e.g. Git Bash, PowerShell):

```bash
# From repo root. In Git Bash use forward slashes; in PowerShell/CMD backslashes are fine.
dotnet build addins/MyDll1Native/MyDll1Native.csproj -c Debug -p:Platform=x64
dotnet build addins/PowerPointAddIn/PowerPointAddIn.csproj -c Debug -p:Platform=x64
dotnet build addins/ExcelAddIn/ExcelAddIn.csproj -c Debug -p:Platform=x64
```

- **Why no COM registration during build?** The add-in projects have `RegisterForComInterop=false`. The .NET Core MSBuild used by `dotnet build` does **not** support the `RegisterAssembly` task (error MSB4803). So we disable it; you register the add-in manually with `regasm /codebase` after building (see “Registering the add-ins” below).
- **Post-build copy:** The add-in projects copy `MyDll1.dll` and OpenCV DLLs only **if** those files exist in `MyDll1\x64\Debug` (or Release). If you have not built the native project (C++ is built in Visual Studio, not by the dotnet CLI), the copy is **skipped** and the add-in build still **succeeds**. To have the native DLLs in the add-in output: build the native project in Visual Studio once, then rebuild the add-in (VS or `dotnet build`).

### Build summary

| How you build | Native MyDll1 built? | Add-in build | MyDll1.dll + OpenCV in add-in output? |
|---------------|----------------------|--------------|--------------------------------------|
| Visual Studio, Build Solution | Yes (first) | Succeeds | Yes |
| dotnet build add-in | No | Succeeds | No (copy skipped) |
| dotnet build add-in | Yes (built in VS earlier) | Succeeds | Yes |

## Registering the add-ins with Office

COM add-ins must be **registered** so PowerPoint/Excel can discover and load them.

### One-time registration (development)

1. Open **Developer Command Prompt for VS 2022** (or any command prompt with `regasm` on PATH).
2. **PowerPoint add-in:**
   ```cmd
   regasm /codebase "path\to\addins\PowerPointAddIn\bin\Debug\PowerPointAddIn.dll"
   ```
3. **Excel add-in:**
   ```cmd
   regasm /codebase "path\to\addins\ExcelAddIn\bin\Debug\ExcelAddIn.dll"
   ```
   Use the **full path** to the built DLL. `/codebase` puts the DLL path in the registry so Office can find it.

4. Add **registry keys** so Office lists the add-in. For PowerPoint (per user):
   - Key: `HKEY_CURRENT_USER\Software\Microsoft\Office\PowerPoint\Addins\PowerPointAddIn.Connect`
   - Values: `Description` (string), `FriendlyName` (string), `LoadBehavior` (DWORD) = 3 (load at startup).

   For Excel:
   - Key: `HKEY_CURRENT_USER\Software\Microsoft\Office\Excel\Addins\ExcelAddIn.Connect`
   - Same values.

   You can create these by hand or use a small script. **LoadBehavior** = 3 means “load when the application starts.”

### Unregister

```cmd
regasm /unregister "path\to\PowerPointAddIn.dll"
regasm /unregister "path\to\ExcelAddIn.dll"
```

Then remove the registry keys under `...\Addins\...` if you added them manually.

## Running

1. After building and registering, start **PowerPoint** or **Excel**.
2. The add-in loads when the app starts (if LoadBehavior=3). It will load `MyDll1.dll` from the **same folder as the add-in DLL** (the post-build copy puts it there).
3. If the native DLL or OpenCV DLLs are missing from that folder, the add-in may fail to load or throw when calling the native code. Ensure you built **MyDll1** first and that the add-in project’s post-build copy ran (addins output folder should contain `MyDll1.dll` and `opencv_world410d.dll` for Debug).

## Optional: project dependencies

So that building an add-in always builds the native DLL first, you can add a solution-level or project-level dependency: **PowerPointAddIn** and **ExcelAddIn** depend on **MyDll1**. In Visual Studio: right-click the add-in project → Project Dependencies → check **MyDll1**. (The add-in does not reference the native project directly; it only needs the built DLL in the expected path so the copy target can run.)

## Troubleshooting

- **MSB4803 (RegisterAssembly not supported):** You built the add-in with `dotnet build` and had `RegisterForComInterop=true`. It is set to `false` so that the dotnet CLI can build the add-ins; register manually with `regasm /codebase` after building.
- **MSB3030 (Could not copy opencv_world410d.dll / MyDll1.dll):** The add-in post-build copy could not find the native DLLs. Build the **MyDll1** (C++) project in Visual Studio first so `MyDll1\x64\Debug` (or Release) contains `MyDll1.dll` and the OpenCV DLLs, then rebuild the add-in. Alternatively, the copy step is now conditional (only runs when the files exist), so the add-in build should succeed even without the native build; you just won’t have the DLLs in the add-in output until you build the native project and rebuild the add-in.
- **Git Bash: “Project file does not exist”:** Paths with backslashes (`addins\...`) can be misinterpreted. Use forward slashes: `addins/PowerPointAddIn/PowerPointAddIn.csproj`.

## Requirements

- **Visual Studio 2022** with .NET desktop development (for C#).
- **Office** — PowerPoint and Excel installed (typically Office 2016+), 64-bit to match the add-in and native DLL.
- **.NET Framework 4.8** on the machine where the add-ins run.

## Next steps (optional)

- Add a **ribbon button** or menu item that calls `MyDll1.GetVersion()` or `MyDll1.GetThirdPartyCheck()` and shows the result in a message box.
- Use the **Office Primary Interop Assemblies** (e.g. Microsoft.Office.Interop.PowerPoint) in the add-in to automate slides or cells and pass data to/from the native engine.
