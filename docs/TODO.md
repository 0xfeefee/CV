# TODO
A brief roadmap.

## Alpha
At first we keep the project `Windows + MSVC` only in order to make progress faster. Goal of the
first stage is to have a working version with enough features to be presentable.

- [ ] Define the 2D engine API.
- [X] Implement the basic ECS, shared by all backends.
- [ ] Implement the Raylib backend
	* [X] Platform window.
	* [ ] Draw rectangles.
	* [ ] Load and play sound.
	* [ ] Keyboard and gamepad input.
	* [ ] Bitmap fonts, text rendering.
	* [ ] Load a texture, draw a subtexture.
- [ ] Implement the Lua frontend
	* [ ] Map lifecycle functions.
	* [ ] Map basic engine functionalities.
	* [ ] Load source relative to current directory.
