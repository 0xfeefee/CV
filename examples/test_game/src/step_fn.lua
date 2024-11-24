
local counter = 0

function game_step()
	if (counter < 100) then
		print("step: " .. counter)
		counter = counter + 1
	end
end
