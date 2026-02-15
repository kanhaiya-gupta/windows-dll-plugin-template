# Office add-ins (C++)

This folder contains the **C++ COM add-in** that plugs the native **PluginEngine** engine into Microsoft PowerPoint and Excel.

**x64 only.** The add-in is built for **x64** only (no Win32). You must use **64-bit Microsoft Office** (PowerPoint, Excel, Word). 32-bit Office cannot load this add-in.

## Project: OfficeAddInCpp

- **One DLL for both apps** — The same `OfficeAddInCpp.dll` is registered as a COM add-in for **PowerPoint** and **Excel** (and optionally Word).
- **IDTExtensibility2** — Implements OnConnection, OnDisconnection, OnAddInsUpdate, OnStartupComplete, OnBeginShutdown.
- **Loads the engine** — In OnConnection it loads `PluginEngine.dll` (from the same folder), calls `PluginEngine_Initialize`. In OnDisconnection it calls `PluginEngine_Shutdown` and frees the DLL.
- **Plugin structure** — Individual plugins are in-process modules implementing **IPlugin** (see `IPlugin.h`). Placeholders:
  - **Plugin_PptExport** — PPT-specific (placeholder).
  - **Plugin_ExcelCharts** — Excel-specific (placeholder).
  - **Plugin_WordTemplates** — Word-specific (placeholder).
  - **Plugin_Common** — Shared (e.g. About, settings; placeholder).
  **PluginHost** registers and loads/unloads them on connect/disconnect. Add ribbon and commands inside each plugin’s `OnLoad`/`OnUnload`.
- **Build** — Depends on **PluginEngine** (engine). Post-build copies `PluginEngine.dll` and OpenCV DLLs into the add-in output folder.

## Build

1. In Visual Studio, set the solution platform to **x64** (not Win32). Build **PluginEngine** first (Debug or Release).
2. Build **OfficeAddInCpp**. Output: `addins\OfficeAddInCpp\x64\Debug\OfficeAddInCpp.dll` (or `x64\Release\`; the engine DLLs are copied there).

Use **Debug | x64** or **Release | x64** only—the project has no Win32 configuration. Same as the engine and test host.

## Register with Office

1. **Register the COM DLL** (run as Administrator, or use HKCU registration which the DLL supports):
   ```cmd
   regsvr32 "full\path\to\addins\OfficeAddInCpp\x64\Debug\OfficeAddInCpp.dll"
   ```
   Or from a command prompt with the add-in folder as current directory:
   ```cmd
   regsvr32 OfficeAddInCpp.dll
   ```

2. **Add Office add-in keys** so PowerPoint and Excel load it:
   - **PowerPoint:** `HKCU\Software\Microsoft\Office\PowerPoint\Addins\OfficeAddInCpp.Connect`
     - `Description` (REG_SZ): e.g. "PluginEngine Office Add-in"
     - `FriendlyName` (REG_SZ): e.g. "PluginEngine Add-in"
     - `LoadBehavior` (DWORD): 3 (load at startup)
   - **Excel:** `HKCU\Software\Microsoft\Office\Excel\Addins\OfficeAddInCpp.Connect` — same values.

3. Start **64-bit** PowerPoint or Excel; the add-in should load and call the engine. (If you have 32-bit Office installed, this x64 add-in will not load—use 64-bit Office.)

## Unregister

```cmd
regsvr32 /u OfficeAddInCpp.dll
```

Then delete the registry keys under `...\Office\PowerPoint\Addins\OfficeAddInCpp.Connect` and `...\Office\Excel\Addins\OfficeAddInCpp.Connect` if you added them manually.

See [docs/PPT_EXCEL_ADDINS.md](../docs/PPT_EXCEL_ADDINS.md) for more detail.
