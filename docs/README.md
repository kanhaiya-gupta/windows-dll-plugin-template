# MyDll1

Plugin DLL built with Visual Studio. This document describes how to build, use, and test the plugin.

## Requirements

- **Visual Studio 2022** (v143 toolset) or compatible
- **Windows SDK** 10.0
- Windows 10/11 (x86 or x64)

## Building

1. Open `MyDll1.sln` in Visual Studio.
2. Select configuration (Debug/Release) and platform (x86/x64).
3. Build Solution (e.g. **Build → Build Solution**).

Output:

- **Debug:** `MyDll1\x64\Debug\MyDll1.dll` (or `Win32\Debug\` for x86)
- **Release:** `MyDll1\x64\Release\MyDll1.dll` (or `Win32\Release\` for x86)

## Using the plugin

1. Copy `MyDll1.dll` to the host application’s plugin directory (or a path the host searches).
2. The host should load the DLL with `LoadLibrary` and resolve:
   - `MyDll1_GetVersion`
   - `MyDll1_Initialize`
   - `MyDll1_Shutdown`
3. Call `MyDll1_Initialize` after load; call `MyDll1_Shutdown` before `FreeLibrary`.

See [API.md](API.md) for the full plugin contract and [ARCHITECTURE.md](ARCHITECTURE.md) for project layout.

## Testing

See the [tests](../tests/README.md) folder. The test host loads the DLL and exercises the exported API.

## Project layout

- **include/** — Public and internal headers (`.h` only).
- **src/** — Implementation only (`.cpp` only).
- **docs/** — This documentation.
- **tests/** — Test host and test instructions.
