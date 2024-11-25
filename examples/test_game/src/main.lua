
require("step_fn") -- Defines: game_step


function game_begin()
	-- expected id: 0
	create_entity({});

	-- expected id: 1
	local e = create_entity({
		x = 192,
		y = 192,
		width = 64,
		height = 64,
		color= {
			r=255
		},
	});

	print(e.id);
	print("game_start!")
end

function game_end()
	print("game_end!")
end
