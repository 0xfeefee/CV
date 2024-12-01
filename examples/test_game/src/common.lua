
local counter = 0

function create_entity(def)
	local entity = cv.create_entity(def)

	if entity.rect then
		entity.rect = cv.get_rect(entity.id)
	end

	if entity.color then
		entity.color = cv.get_color(entity.id)
	end

	if entity.velocity then
		entity.velocity = cv.get_velocity(entity.id)
	end

	if entity.texture then
		entity.texture = cv.get_texture(entity.id)
	end

	return entity
end
