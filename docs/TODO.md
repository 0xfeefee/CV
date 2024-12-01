# TODO
A brief roadmap.

## Alpha
At first we keep the project `Windows + MSVC` only in order to make progress faster. Goal of the
first stage is to have a working version with enough features to be presentable. In this stage
we are allowed to make suboptimal technical choices, though it should not be all of them.

- [ ] Define the 2D engine API.
- [X] Implement the basic ECS, shared by all backends.
- [ ] Implement the Raylib backend
	* [X] Platform window.
	* [X] Draw rectangles.
	* [X] Load and play sound.
	* [X] Keyboard and gamepad input.
	* [ ] Bitmap fonts, text rendering.
	* [X] Load a texture, draw a subtexture.
- [ ] Define the frontend hooks/API.
- [ ] Implement the Lua frontend.
	* [X] Map lifecycle functions.
	* [ ] Map basic engine functionalities.
	* [ ] Load source relative to current directory.

## Beta
Render the game within a 3D scene, ensure all features necessary are supported.

- [ ] Add support for loading .obj models (or .dae for entire scene at once?)
- [ ] Model the scene in blender.
- [ ] Add simple the CRT shader for the virtual screen.
- [ ] Render & test.
- [ ] Implement a simple demo game.

## Gamma
Remove any rough edges, optimize, restructure when necessary, add more frontend/backend options.

- [ ] Add Wren frontend
- [ ] Add Python/Ruby or JS frontend.
- [ ] Add DirectX backend.
- [ ] Add the custom software rendered backend?

## The end?
What is the end goal?
