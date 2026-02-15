# Architecture

## Directory layout

```
MyDll1/
├── include/           # Headers only
│   ├── framework.h    # Windows headers
│   ├── pch.h          # Precompiled header
│   └── MyDll1_API.h   # Public plugin API
├── src/               # Implementation only (.cpp)
│   ├── pch.cpp        # PCH source
│   ├── dllmain.cpp    # DLL entry point
│   └── plugin_impl.cpp# Plugin API implementation
├── docs/              # Documentation
└── tests/             # Test host and test docs
```

- **include/** holds all headers. No `.cpp` files here.
- **src/** holds all `.cpp` files. No headers here; sources `#include` from `include/` via project include path.

## Layers

1. **API (include/MyDll1_API.h)**  
   C-style, stable contract: version and lifecycle (`GetVersion`, `Initialize`, `Shutdown`). Exported via `MYDLL1_API` (dllexport when building the DLL, dllimport when building the host).

2. **DLL entry (src/dllmain.cpp)**  
   Minimal `DllMain`: only process/thread attach/detach; optional storage of `hModule`. No allocation or heavy work.

3. **Implementation (src/plugin_impl.cpp)**  
   Implements the functions declared in `MyDll1_API.h`. Internal logic can use C++; the host sees only the C API.

## Build

The project adds `$(ProjectDir)include` to **AdditionalIncludeDirectories**, so every `#include "pch.h"` or `#include "MyDll1_API.h"` resolves from `include/` regardless of which `.cpp` in `src/` is compiling.

Precompiled header is created from `src/pch.cpp` using `pch.h` from `include/`.
