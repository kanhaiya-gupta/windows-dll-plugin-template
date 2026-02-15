# Plugin API

PluginEngine exposes a C API so that any host can load the DLL and call the plugin without sharing C++ ABI or CRT.

## Header

Include `PluginEngine_API.h` when building the host. When building the DLL, define `PLUGINENGINE_EXPORTS` so the same header exports the symbols.

## Version

- **PLUGINENGINE_VERSION_MAJOR / MINOR / PATCH** — Integer version components.
- **PluginEngine_GetVersion()** — Returns a single `unsigned int`: `(major << 16) | (minor << 8) | patch`. Example: 1.0.0 → `0x00010000`.

## Lifecycle

| Function | Description |
|----------|-------------|
| **PluginEngine_GetVersion** | Returns encoded version. Safe to call anytime after load. |
| **PluginEngine_Initialize** | One-time setup. Call once after `LoadLibrary`, before other plugin use. Returns 0 on success, non-zero on failure. |
| **PluginEngine_Shutdown** | Cleanup. Call once before `FreeLibrary`. |

## Calling convention

All functions use the default calling convention (e.g. `__cdecl` on x86/x64 with MSVC). No COM or C++ types cross the boundary.

## Example (host)

```c
#include "PluginEngine_API.h"
#include <windows.h>

HMODULE h = LoadLibraryW(L"PluginEngine.dll");
if (!h) { /* fail */ }

typedef unsigned int (__cdecl *GetVersionFn)(void);
typedef int (__cdecl *InitFn)(void);
typedef void (__cdecl *ShutdownFn)(void);

GetVersionFn getVer = (GetVersionFn)GetProcAddress(h, "PluginEngine_GetVersion");
InitFn init = (InitFn)GetProcAddress(h, "PluginEngine_Initialize");
ShutdownFn shutdown = (ShutdownFn)GetProcAddress(h, "PluginEngine_Shutdown");

if (getVer) { unsigned int v = getVer(); /* ... */ }
if (init && init() != 0) { FreeLibrary(h); return; }
// ... use plugin ...
if (shutdown) shutdown();
FreeLibrary(h);
```
