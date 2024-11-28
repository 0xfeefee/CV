
require("common") -- Defines: the Lua API which will be preloaded in future versions.

player = nil

function game_begin()
	-- expected id: 0, no components
	create_entity({});

	test = cv.load_texture("tilemap");

	-- First pixel guy in the { tilemap }, scale: 16x
	player = create_entity({
		x= 192,
		y= 192,
		width= 128,
		height= 128,
		r= 255,
		g= 0,
		b= 0,
		a= 255,
		vspeed= 40,
		hspeed= 40,
		texture_id= test.id,
		texture_x= 45,
		texture_y= 54,
		texture_width= 8,
		texture_height= 8
	});

	player.rect.x = 0;

	-- Distorted tilemap
	local something = create_entity({
		x= 256,
		y= 256,
		width= test.rect.width*2,
		height= test.rect.height*2,
		texture_id= test.id
	});
end

function game_step()
	if player.color.r > 0 then
		player.color.r = player.color.r - 1
	end

	if cv.is_key_pressed(Key.A) then
		print("A was pressed!")
		player.velocity.x = -1 * player.velocity.x
	end

	if cv.is_key_pressed(Key.B) then
		print("B was pressed!")
		player.velocity.y = -1 * player.velocity.y
	end
end

function game_end()
	print("game_end!")
end
