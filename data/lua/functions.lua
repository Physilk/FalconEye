
function createChessBoard(scene, width, height)
    local halfWidth = width / 2
    local halfHeight = height / 2
    local mat_tile_dark = Material(Color(0.1, 0.1, 0.1, 1), 32, 0.1, 1)
    local mat_tile_light = Material(Color(0.8, 0.8, 0.8, 1), 32, 0.1, 1)

    local materials = {mat_tile_dark, mat_tile_light}
    local color_index = 0
    local last_row_color_index = 1

    for x = 0, width do
        color_index = (last_row_color_index + 1) % 2
        for y = 0, height do
            local p1 = Point(x - halfWidth - 0.5, 0, y - halfHeight - 0.5)
            local p2 = Point(x - halfWidth + 0.5, 0, y - halfHeight - 0.5)
            local p3 = Point(x - halfWidth + 0.5, 0, y - halfHeight + 0.5)
            local p4 = Point(x - halfWidth - 0.5, 0, y - halfHeight + 0.5)

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