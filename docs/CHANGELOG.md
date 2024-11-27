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
- 2024-11-19: Dropped support for `Clang` compiler in order to make development easier at this
  stage. We can re-introduce it later.
- 2024-11-20: Added the stub of a basic ECS.
- 2024-11-21: ECS template implementation fixes.
- 2024-11-22: Added the stub for front-end, refactored ECS.

## v0.4.0
- 2024-11-23: Added the `Lua` and `sol` dependency.
- 2024-11-24: Changed the build script to enable `frontend` selection, between: `{ Lua, Wren }`.
- 2024-11-24: Implemented the basic `Lua` frontend.
- 2024-11-25: Adding bindings for ECS/basic engine functionality to Lua.
- 2024-11-26: Implemented POC engine features for Lua frontend.
- 2024-11-26: Offscreen rendering stub, 3D billboard stub.

## v0.5.0
- 2024-11-27: Adding the texture rendering API/systems.
