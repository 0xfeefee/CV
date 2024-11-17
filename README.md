# CV (v0.1)
This is just a basic demo of programming skills, not a real product.

## Idea (subject to change)
- A tiny 2D game engine.
- Backend agnostic.
- Very limited, like Pico-8.
- Scripting with Lua or something else (agnostic?).
- Game runs on a virtual screen in 3D space (Inscryption vibes).

## Project structure & setup
- [bin/](./bin): build output directory.
- [build/](./build): CMake build scripts.
- [scripts/](./scripts): Scripts you can call to build/run the project.
- [docs/](./docs): project documentation (CHANGELOG, screenshots, profiler/debugger files, etc.)
- [src/](./src): all project source files.
- [vendor/](./vendor): all 3rd party source code and libraries.

### Scripts
- [scripts/config.bat](./scripts/config.bat): Generates the build files with respect to the
given configuration variables.
- [scripts/build.bat](./scripts/build.bat): Builds the project, must run `config` first!
- [scripts/run.bat](./scripts/run.bat): Runs the project.

### Dependencies
In order to make builds as pleasant as possible all dependencies are located within the project
as a git submodule. Exact version of every dependency is hosted as a "fork" to ensure it's easy
to find:

- [spdlog-v1.14.1](https://github.com/0xfeefee/spdlog-v1.14.1)
