
require("common") -- Defines: the Lua API which will be preloaded in future versions.

player = nil

function game_begin()
	-- expected id: 0
	create_entity({});

	-- expected id: 1
	player = create_entity({
		x= 192,
		y= 192,
		width= 64,
		height=  64,
		r= 255,
		g= 0,
		b= 0,
		a= 255,
		vspeed= 40,
		hspeed= 40
	});

	player.rect.x = 0

	local e2 = create_entity({
		x= 256, y= 64,
		width= 64, height= 32,
		r= 255,
		hspeed= 50
	});

	print(e.id);
	print("game_start!")
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
