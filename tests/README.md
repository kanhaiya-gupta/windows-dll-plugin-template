# Tests

This folder contains the test host for MyDll1. The test host is a small console application that loads the DLL and calls the plugin API to verify load, version, initialize, and shutdown.

## Test host project

- **MyDll1TestHost** — Win32 console app that:
  1. Builds after the MyDll1 project (project dependency).
  2. Copies `MyDll1.dll` into its output directory in a post-build step.
  3. Loads the DLL with `LoadLibrary`, resolves `MyDll1_GetVersion`, `MyDll1_Initialize`, `MyDll1_Shutdown`, runs them, then `FreeLibrary`.

## How to run

1. Build the solution (Debug or Release, x86 or x64). Ensure **MyDll1** and **MyDll1TestHost** both build.
2. Set **MyDll1TestHost** as the startup project (right-click → Set as Startup Project), or run the built executable from the test host output folder.
3. Run (F5 or Ctrl+F5). You should see exit code 0 and no errors.

## Expected behavior

- DLL loads successfully.
- `MyDll1_GetVersion()` returns version (e.g. `0x00010000` for 1.0.0).
- `MyDll1_Initialize()` returns 0.
- `MyDll1_Shutdown()` runs without crashing.
- Process exits with code 0.

**"Not a valid Win32 application"** means the DLL and the test host were built for different architectures (e.g. 32‑bit host with 64‑bit DLL). In Visual Studio, pick the **same** platform for the whole solution (e.g. **x64** or **Win32**) and rebuild both MyDll1 and MyDll1TestHost, then run again.

If the DLL is not found, ensure you built the same configuration (e.g. Debug x64) for both MyDll1 and MyDll1TestHost; the post-build copy uses the same platform and configuration.
