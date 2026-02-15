# PluginEngine

Plugin engine DLL built with Visual Studio. This document describes how to build, use, and test the engine.

## Requirements

- **Visual Studio 2022** (v143 toolset) or compatible
- **Windows SDK** 10.0
- Windows 10/11 (x64). This solution is **x64 only** (no 32-bit/Win32 platform).

## Building

1. Open `PluginEngine.sln` in Visual Studio.
2. Select configuration (Debug/Release) and platform **x64**.
3. Build Solution (e.g. **Build → Build Solution**).

Output:

- **Debug:** `PluginEngine\x64\Debug\PluginEngine.dll`
- **Release:** `PluginEngine\x64\Release\PluginEngine.dll`

## Using the plugin

1. Copy `PluginEngine.dll` to the host application’s plugin directory (or a path the host searches).
2. The host should load the DLL with `LoadLibrary` and resolve:
   - `PluginEngine_GetVersion`
   - `PluginEngine_Initialize`
   - `PluginEngine_Shutdown`
3. Call `PluginEngine_Initialize` after load; call `PluginEngine_Shutdown` before `FreeLibrary`.

See [API.md](API.md) for the full plugin contract and [ARCHITECTURE.md](ARCHITECTURE.md) for project layout.

## Testing

See the [tests](../tests/README.md) folder. The test host loads the DLL and exercises the exported API.

## Project layout

- **include/** — Public and internal headers (`.h` only).
- **src/** — Implementation only (`.cpp` only).
- **docs/** — This documentation.
- **tests/** — Test host and test instructions.
