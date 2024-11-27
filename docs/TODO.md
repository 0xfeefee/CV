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
	* [ ] Load and play sound.
	* [ ] Keyboard and gamepad input.
	* [ ] Bitmap fonts, text rendering.
	* [X] Load a texture, draw a subtexture.
- [ ] Define the frontend hooks/API.
- [ ] Implement the Lua frontend.
	* [X] Map lifecycle functions.
	* [ ] Map basic engine functionalities.
	* [ ] Load source relative to current directory.
