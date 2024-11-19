# CHANGELOG

## v0.1.0
- 2024-11-16: Created the project.
- 2024-11-16: Compile and run the "Hello, World!" program.
- 2024-11-16: Added the `CMake` build script.
- 2024-11-16: Added the batch scripts to build/configure/run on Windows.
- 2024-11-17: Created the pre-complied header file `base.pch.hpp`.
- 2024-11-17: Added the base aliases for fixed size integer types to the `pch`.
- 2024-11-17: Added `spdlog-v1.14` as dependency to the project.
- 2024-11-17: Added logging utilities to the `pch`, implementation uses `spdlog`.

## v0.2.0
- 2024-11-18: Added the basic engine structure code.
- 2024-11-18: Fixed linker errors on missing logger implementations when `PROJECT_ENABLE_LOGS`
  is disabled.
- 2024-11-18: Added the new config option `PROJECT_ENGINE_BACKEND` which allows selection of
  engine backend (implementation).

## v0.3.0
- 2024-11-19: Added the Raylib dependency, as submodule.
- 2024-11-19: Implemented "Hello Window" with Raylib.
- 2024-11-19: Renamed `get_instance` to `get_context` to better reflect it's intended use.
