# windows-dll-plugin-template

A Windows x64 plugin DLL built with Visual Studio. Exposes a C API for version and lifecycle (`Initialize` / `Shutdown`). Use as a **template**: clone and rename the project to bootstrap a new plugin that uses third-party libraries (e.g. OpenCV).

## Quick start

1. Open **MyDll1.sln** in Visual Studio 2022.
2. Choose configuration (Debug/Release) and platform **x64** (this solution is x64-only).
3. Build Solution. The DLL is produced under `MyDll1\x64\<Configuration>\MyDll1.dll`.
4. To run tests: set **MyDll1TestHost** as the startup project and run (F5). The test host loads the DLL and exercises the API.

## Project layout

| Folder    | Contents                          |
|-----------|-----------------------------------|
| **MyDll1/** | DLL project: `include/` (headers), `src/` (.cpp only) |
| **docs/**  | [README](docs/README.md), [Architecture](docs/ARCHITECTURE.md), [API](docs/API.md) |
| **tests/** | Test host app and [test instructions](tests/README.md) |

## Requirements

- Visual Studio 2022 (v143 toolset)
- Windows SDK 10.0
- Windows 10/11

For full build instructions, plugin usage, and API details, see **[docs/](docs/)**.

## Use as a DLL template

You can use this repo as a **template** for new Windows plugin DLLs:

- **Structure:** `include/` (public C API), `src/` (implementation), `docs/`, `tests/` (test host), `third_party/` (dependencies).
- **Third-party:** Add libraries under `third_party/` and follow [third_party/README.md](third_party/README.md) for wiring and for handling packs that ship only DLLs (generate import libs).
- **Rename:** Create a new solution/project or rename MyDll1 → YourPlugin (solution, project, folder, namespaces, and API prefix).
- **Docs:** [docs/logs.md](docs/logs.md) and [third_party/README.md](third_party/README.md) document the problems we hit (missing .lib, unresolved externals, .def parsing) and how to add new third-party software.
