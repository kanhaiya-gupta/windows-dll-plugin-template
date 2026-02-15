# Tests

This folder contains the test host for PluginEngine. The test host is a small console application that loads the DLL and calls the plugin API to verify load, version, initialize, and shutdown.

## Test host project

- **PluginEngineTestHost** — x64 console app that:
  1. Builds after the PluginEngine project (project dependency).
  2. Copies `PluginEngine.dll` into its output directory in a post-build step.
  3. Loads the DLL with `LoadLibrary`, resolves `PluginEngine_GetVersion`, `PluginEngine_Initialize`, `PluginEngine_Shutdown`, runs them, then `FreeLibrary`.

## How to run

1. Build the solution (Debug or Release, **x64** only). Ensure **PluginEngine** and **PluginEngineTestHost** both build.
2. Set **PluginEngineTestHost** as the startup project (right-click → Set as Startup Project). Do not set PluginEngine or OfficeAddInCpp as startup—they are DLLs; running a DLL produces "Win32 is not a valid application."
3. Run (F5 or Ctrl+F5). You should see exit code 0 and no errors.

## Expected behavior

- DLL loads successfully.
- `PluginEngine_GetVersion()` returns version (e.g. `0x00010000` for 1.0.0).
- `PluginEngine_Initialize()` returns 0.
- `PluginEngine_Shutdown()` runs without crashing.
- Process exits with code 0.

### Fix: "PluginEngine.dll is not a valid Win32 application"

**Why it happens:** Visual Studio is trying to **run the DLL** (PluginEngine or OfficeAddInCpp) instead of the **.exe** (PluginEngineTestHost). You cannot run a DLL like an app; Windows then reports it as "not a valid Win32 application." The solution is x64-only—nothing is building as 32-bit.

**Do this (in order):**

1. **Set the startup project to the .exe**
   - In **Solution Explorer**, click **PluginEngineTestHost** (the project under `tests`, not the solution name).
   - Right‑click **PluginEngineTestHost** → **Set as Startup Project**.
   - The startup project should show in **bold**. If **PluginEngine** or **OfficeAddInCpp** is bold, you are still set to run the DLL—you cannot run a DLL; use the test host to test the engine.

2. **Confirm platform is x64**
   - Toolbar: set **Debug** (or Release) and **x64** (not Win32).
   - **Build** → **Configuration Manager** → **Active solution platform** must be **x64**.

3. **Build, then run**
   - **Build** → **Build Solution**.
   - Press **F5** (or **Ctrl+F5**). The process that starts must be **PluginEngineTestHost.exe** (64‑bit), not the DLL.

4. **If it still says "not a valid Win32 application"**
   - Close Visual Studio.
   - Delete the **.vs** folder in your solution folder (e.g. `MyDll1\.vs`). This clears the stored startup project and platform.
   - Reopen **PluginEngine.sln**.
   - Set platform to **x64**, set **PluginEngineTestHost** as startup (step 1), then Build and F5.

If the DLL is not found, ensure you built the same configuration (e.g. Debug x64) for both PluginEngine and PluginEngineTestHost; the post-build copy uses the same platform and configuration.
