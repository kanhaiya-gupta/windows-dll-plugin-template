# PowerPoint and Excel plugins (C++)

This document describes **Microsoft PowerPoint** and **Excel** plugins built on top of the native engine, **all in C++**.

**Current state:** The **OfficeAddInCpp** DLL and the native **PluginEngine.dll** are the starting point. The add-in loads the engine on connect and shuts it down on disconnect. The finished plugin will be built on this base: add ribbon UI, menu actions, and features that use the native DLL.

## How it fits

- **PluginEngine (engine)** — C++ DLL with C API: version, init/shutdown, third-party (e.g. OpenCV). No Office-specific code.
- **OfficeAddInCpp** — C++ COM add-in that implements **IDTExtensibility2**. One DLL for both PowerPoint and Excel. It loads **PluginEngine.dll** and calls its C API.

```
PowerPoint / Excel  →  OfficeAddInCpp.dll (C++ COM)  →  LoadLibrary  →  PluginEngine.dll (engine)
```

## What’s in addins/

| Item | Role |
|------|------|
| **OfficeAddInCpp** | C++ COM add-in DLL. Implements IDTExtensibility2; OnConnection loads PluginEngine.dll and calls PluginEngine_Initialize; OnDisconnection calls PluginEngine_Shutdown and FreeLibrary. Post-build copies PluginEngine.dll and OpenCV DLLs to add-in output. |

Single project, one DLL, registered for both PowerPoint and Excel.

## Build

1. Open **PluginEngine.sln** in Visual Studio 2022.
2. Select **Debug | x64** (or Release | x64).
3. **Build → Build Solution.**

Build order: **PluginEngine** (engine) first, then **OfficeAddInCpp**. The add-in project depends on PluginEngine, so the engine is built first. The add-in’s post-build step copies `PluginEngine.dll` and the OpenCV DLLs from `PluginEngine\x64\Debug` (or Release) into `addins\OfficeAddInCpp\x64\Debug` (or Release).

If the engine hasn’t been built yet, the copy is skipped and the add-in still builds; you just won’t have the engine DLLs in the add-in folder until you build the engine and rebuild the add-in.

## Registering the add-in with Office

1. **Register the COM DLL** (once per machine or user):
   ```cmd
   regsvr32 "full\path\to\addins\OfficeAddInCpp\x64\Debug\OfficeAddInCpp.dll"
   ```
   This registers the COM class and ProgId **OfficeAddInCpp.Connect**. No .NET or regasm; it’s a native COM DLL.

2. **Add Office add-in keys** so PowerPoint and Excel load it:
   - **PowerPoint:** Create key `HKEY_CURRENT_USER\Software\Microsoft\Office\PowerPoint\Addins\OfficeAddInCpp.Connect`
     - `Description` (REG_SZ): e.g. "PluginEngine Office Add-in"
     - `FriendlyName` (REG_SZ): e.g. "PluginEngine Add-in"
     - `LoadBehavior` (DWORD): **3** (load at startup)
   - **Excel:** Create key `HKEY_CURRENT_USER\Software\Microsoft\Office\Excel\Addins\OfficeAddInCpp.Connect` with the same values.

3. Start **PowerPoint** or **Excel**. The add-in should load; it will load PluginEngine.dll from the same folder as OfficeAddInCpp.dll.

### Unregister

```cmd
regsvr32 /u "full\path\to\OfficeAddInCpp.dll"
```

Then remove the keys under `...\Office\PowerPoint\Addins\OfficeAddInCpp.Connect` and `...\Office\Excel\Addins\OfficeAddInCpp.Connect` if you added them manually.

## Running

1. After building and registering, start PowerPoint or Excel.
2. With LoadBehavior=3, the add-in loads at startup. It loads **PluginEngine.dll** from the same directory as **OfficeAddInCpp.dll** (the post-build copy puts it there).
3. If PluginEngine.dll or the OpenCV DLLs are missing from that folder, the add-in may load but fail when it calls the engine. Ensure the engine project was built and the add-in was rebuilt so the copy step ran.

## Troubleshooting

- **Add-in doesn’t load:** Check that regsvr32 succeeded and that the registry keys under `...\Office\PowerPoint\Addins\OfficeAddInCpp.Connect` (and Excel) exist with LoadBehavior=3. Use 64-bit regsvr32 for 64-bit Office (e.g. from “x64 Native Tools Command Prompt”).
- **Engine not found / crash in OnConnection:** Ensure **PluginEngine.dll** (and for Debug, **opencv_world410d.dll**) are in the **same folder** as OfficeAddInCpp.dll. Build the engine first, then rebuild the add-in so the post-build copy runs.
- **MSB3030 (Could not copy PluginEngine.dll):** Build **PluginEngine** first so `PluginEngine\x64\Debug` (or Release) contains the DLLs; then rebuild OfficeAddInCpp. The copy is conditional, so the add-in can build without the engine, but you need the engine built for the copy to happen.

## Requirements

- **Visual Studio 2022** with C++ desktop development (v143 toolset).
- **Office** — PowerPoint and Excel (typically Office 2016+), **64-bit** to match the x64 build.
- No .NET required for the add-in; it’s all C++.

## Next steps (optional)

- Add **IRibbonExtensibility** in the C++ add-in for a custom ribbon tab and buttons that call the engine (e.g. GetVersion, GetThirdPartyCheck).
- Extend the **engine** C API and call new functions from the add-in when the user clicks a button.
