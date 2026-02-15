# Office add-ins (PowerPoint and Excel)

This folder contains the **COM add-in** projects that plug the native **MyDll1** DLL into Microsoft PowerPoint and Excel.

## Projects

| Project | Description |
|---------|-------------|
| **MyDll1Native** | C# P/Invoke wrapper for `MyDll1.dll`. Exposes `GetVersion()`, `Initialize()`, `Shutdown()`, `GetThirdPartyCheck()`. |
| **PowerPointAddIn** | COM add-in for PowerPoint. Loads the native DLL on connect and shuts it down on disconnect. |
| **ExcelAddIn** | COM add-in for Excel. Same behavior. |

All target **.NET Framework 4.8**, **x64** (to load the x64 native DLL).

---

## Build

### Option A: Visual Studio (recommended for full solution)

1. Open **MyDll1.sln** in Visual Studio 2022.
2. Select **Debug \| x64** (or Release \| x64).
3. **Build → Build Solution.**

This builds **MyDll1** (native) first, then **MyDll1Native**, then the add-ins. The add-in projects have a post-build step that **copies** `MyDll1.dll` and the OpenCV DLLs from `MyDll1\x64\Debug` (or Release) into the add-in output folder so Office can load them.

### Option B: dotnet CLI (add-ins only)

You can build the C# add-ins from the command line **without** building the C++ project:

```bash
# From repo root. Use forward slashes in Git Bash; backslashes are fine in PowerShell/CMD.
dotnet build addins/MyDll1Native/MyDll1Native.csproj -c Debug -p:Platform=x64
dotnet build addins/PowerPointAddIn/PowerPointAddIn.csproj -c Debug -p:Platform=x64
dotnet build addins/ExcelAddIn/ExcelAddIn.csproj -c Debug -p:Platform=x64
```

- **COM registration at build:** The add-in projects use `RegisterForComInterop=false` so that `dotnet build` works. The .NET Core MSBuild used by the dotnet CLI does **not** support the `RegisterAssembly` task (COM registration). Register the add-in manually with `regasm /codebase` after building (see below).
- **Copy of native DLLs:** The post-build copy runs only **if** the files exist. If you have not built the native **MyDll1** project (in Visual Studio), `MyDll1.dll` and the OpenCV DLLs will not be in `MyDll1\x64\Debug`, so the copy is skipped and the add-in build still **succeeds**. To get the native DLLs into the add-in output: build the native project in Visual Studio, then rebuild the add-in (in VS or with `dotnet build`).

### Summary

| Build with | Native MyDll1 built first? | Add-in build | Native DLLs in add-in output? |
|------------|----------------------------|--------------|--------------------------------|
| Visual Studio, Build Solution | Yes (automatic) | Succeeds | Yes (post-build copy) |
| dotnet build add-in only | No | Succeeds | No (copy skipped) |
| dotnet build add-in only | Yes (built in VS earlier) | Succeeds | Yes (copy runs) |

---

## Register with Office

Register the add-in DLL with `regasm /codebase`, then add the registry keys under `HKCU\Software\Microsoft\Office\PowerPoint\Addins` (or `...\Excel\Addins`) so Office loads it. See [docs/PPT_EXCEL_ADDINS.md](../docs/PPT_EXCEL_ADDINS.md) for full steps.
