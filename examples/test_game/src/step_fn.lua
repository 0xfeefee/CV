
local counter = 0

function game_step()
	if (counter < 1000) then
		counter = counter + 1
		set_clear_color(Color.new(counter % 255, 255 - (counter % 255), 100))
	else
		set_clear_color(Color.new(0, 0, 0, 0))
	end
end
