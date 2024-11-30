
require("common") -- Defines: the Lua API which will be preloaded in future versions.

player 		= nil
click  	    = nil
primary_fnt = nil

function game_begin()
	-- expected id: 0, no components
	create_entity({});

	test = cv.load_texture("tilemap");
	click = cv.load_sound("click", 1.0, 1.0);
	primary_fnt = cv.load_font("RedHatMono-Regular", 16);

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

	local ent2 = create_entity({
		x=100,
		y=100,
		width=1,
		height=1,
		text="Hello, World!",
		font=primary_fnt
	});
end

function game_step()
	if cv.is_key_pressed(Key.A) then
		player.velocity.x = -1 * player.velocity.x
		cv.play_sound(click);
	end

	if cv.is_key_pressed(Key.B) then
		cv.play_sound(click)
		player.velocity.y = -1 * player.velocity.y
		cv.play_sound(click);
	end
end

function game_end()
	print("game_end!")
end
