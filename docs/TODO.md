# TODO
A brief roadmap (not entirely accurate).

## Alpha
At first we keep the project `Windows + MSVC` only in order to make progress faster. Goal of the
first stage is to have a working version with enough features to be presentable. In this stage
we are allowed to make sub-optimal technical choices, though it should not be all of them.

- [X] Define the 2D engine API.
- [X] Implement the basic ECS, shared by all backends.
- [X] Implement the Raylib backend (first pass)
	* [X] Platform window.
	* [X] Draw rectangles.
	* [X] Load and play sound.
	* [X] Keyboard and gamepad input.
	* [X] Bitmap fonts, text rendering.
	* [X] Load a texture, draw a subtexture.
- [X] Define the frontend hooks/API.
- [X] Implement the Lua frontend. (first pass)
	* [X] Map lifecycle functions.
	* [X] Map basic engine functionalities.
	* [X] Load source relative to current directory.

## Beta
Alpha is a rough draft of the 2D engine and structure of the program, Beta has to build on top of
it and define the edges. Here we figure out the exact scope of the application and focus on
building in that direction.

- [ ] Render the game within a 3D scene.
	* [ ] Render the output of 2d engine to an offscreen texture.
	* [ ] Add support for loading .obj models (or .dae for entire scene at once?)
	* [ ] Model the scene in blender.
	* [ ] Add simple the CRT shader for the virtual screen.
	* [ ] Render & test.
	* [ ] Implement a simple demo game.

## Gamma
Beta stage defines the scope and implements support for features within that scope. Gamma removes
any rough edges and polishes what Beta stage defined.

- [ ] Clean up the structure.
- [ ] Profile and optimize the shared core.
- [ ] Add Wren frontend
- [ ] Add Python/Ruby or JS frontend.
- [ ] Add DirectX backend.
- [ ] Add the custom software rendered backend?
- [ ] Add the server, store games on cloud?

## TBD...
...
