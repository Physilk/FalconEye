
function createChessBoard(scene, width, height, tile_size, material_1, material_2)
	if tile_size == nil then
		tile_size = 1
	end
    local halfWidth = (width * tile_size) / 2
    local halfHeight = (height * tile_size) / 2
    local halfeTileSize = tile_size / 2

    local materials = {material_1, material_2}
    local color_index = 0
    local last_row_color_index = 1

    for x = 0, width do
        color_index = (last_row_color_index + 1) % 2
        for y = 0, height do
            local p1 = Point(x * tile_size - halfWidth - halfeTileSize, 0, y * tile_size - halfHeight - halfeTileSize)
            local p2 = Point(x * tile_size - halfWidth + halfeTileSize, 0, y * tile_size - halfHeight - halfeTileSize)
            local p3 = Point(x * tile_size - halfWidth + halfeTileSize, 0, y * tile_size - halfHeight + halfeTileSize)
            local p4 = Point(x * tile_size - halfWidth - halfeTileSize, 0, y * tile_size - halfHeight + halfeTileSize)

            scene:addObject(Triangle(p4, p3, p2, materials[color_index + 1]))
            scene:addObject(Triangle(p4, p2, p1, materials[color_index + 1]))
            
            color_index = color_index + 1
            color_index = color_index % 2
        end
        last_row_color_index = last_row_color_index + 1
		last_row_color_index = last_row_color_index % 2
    end
end

function createPointLigth(position, range, color)
    return PointLight(position, range, color, AttenuationParameters(1, 4.5/range, 75/(range*range)))
end

function pauseUntilEnter()
    io.output():write("Press any key to continue")
    io.input():read(1)
end

function tablelength(T)
  local count = 0
  for _ in pairs(T) do count = count + 1 end
  return count
end

function colorToString(c)
  return "r=" .. c.r .. " g=" .. c.g .. " b=" .. c.b .. " a=" .. c.a 
end
