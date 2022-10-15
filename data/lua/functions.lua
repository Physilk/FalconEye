
function colorToString(c)
  return "r=" .. c.r .. " g=" .. c.g .. " b=" .. c.b .. " a=" .. c.a 
end

local Objects = require 'data/lua/objects'

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

function createMontain(min, max, subdivision_x, subdivision_y, variationParams, inMaterial)
  local pointArray = {}
  local dx = (max.x - min.x) / subdivision_x
  local dy = (max.y - min.y) / subdivision_y

  math.randomseed(os.time())
  --math.random();
  for i = 0, subdivision_x do
    pointArray[i] = {}
    for j = 0, subdivision_y do
      pointArray[i][j] = {x = min.x + (dx * i), y = min.y + (dy * j), z = min.z + math.random() * variationParams.variationZ}
    end
  end
  
  local triangleIndex = 0;
  local triangles = {}
  for i = 0, subdivision_x - 1 do
    for j = 0, subdivision_y - 1 do
      if pointArray[i+1] then
        if pointArray[i+1][j+i] then
          triangles[triangleIndex] = Objects.make_triangle({
          v0 = FalconEye.Point(pointArray[i  ][j  ].x, pointArray[i  ][j  ].y, pointArray[i  ][j  ].z),
          v1 = FalconEye.Point(pointArray[i+1][j+1].x, pointArray[i+1][j+1].y, pointArray[i+1][j+1].z),
          v2 = FalconEye.Point(pointArray[i  ][j+1].x, pointArray[i  ][j+1].y, pointArray[i  ][j+1].z),
          material = inMaterial})
          triangles[triangleIndex].bCastShadow = false
          triangleIndex = triangleIndex + 1
          triangles[triangleIndex] = Objects.make_triangle({
          v0 = FalconEye.Point(pointArray[i  ][j  ].x, pointArray[i  ][j  ].y, pointArray[i  ][j  ].z),
          v1 = FalconEye.Point(pointArray[i+1][j  ].x, pointArray[i+1][j  ].y, pointArray[i+1][j  ].z),
          v2 = FalconEye.Point(pointArray[i+1][j+1].x, pointArray[i+1][j+1].y, pointArray[i+1][j+1].z),
          material = inMaterial})
          triangles[triangleIndex].bCastShadow = false
          triangleIndex = triangleIndex + 1
        end
      end
    end
  end
  
  return triangles
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

function tprint (tbl, indent)
  if not indent then indent = 0 end
  local toprint = string.rep(" ", indent) .. "{\r\n"
  indent = indent + 2 
  for k, v in pairs(tbl) do
    toprint = toprint .. string.rep(" ", indent)
    if (type(k) == "number") then
      toprint = toprint .. "[" .. k .. "] = "
    elseif (type(k) == "string") then
      toprint = toprint  .. k ..  "= "   
    end
    if (type(v) == "number") then
      toprint = toprint .. v .. ",\r\n"
    elseif (type(v) == "string") then
      toprint = toprint .. "\"" .. v .. "\",\r\n"
    elseif (type(v) == "table") then
      toprint = toprint .. tprint(v, indent + 2) .. ",\r\n"
    else
      toprint = toprint .. "\"" .. tostring(v) .. "\",\r\n"
    end
  end
  toprint = toprint .. string.rep(" ", indent-2) .. "}"
  return toprint
end


function print_table(node)
  local cache, stack, output = {},{},{}
  local depth = 1
  local output_str = "{\n"

  while true do
      local size = 0
      for k,v in pairs(node) do
          size = size + 1
      end

      local cur_index = 1
      for k,v in pairs(node) do
          if (cache[node] == nil) or (cur_index >= cache[node]) then

              if (string.find(output_str,"}",output_str:len())) then
                  output_str = output_str .. ",\n"
              elseif not (string.find(output_str,"\n",output_str:len())) then
                  output_str = output_str .. "\n"
              end

              -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
              table.insert(output,output_str)
              output_str = ""

              local key
              if (type(k) == "number" or type(k) == "boolean") then
                  key = "["..tostring(k).."]"
              else
                  key = "['"..tostring(k).."']"
              end

              if (type(v) == "number" or type(v) == "boolean") then
                  output_str = output_str .. string.rep('\t',depth) .. key .. " = "..tostring(v)
              elseif (type(v) == "table") then
                  output_str = output_str .. string.rep('\t',depth) .. key .. " = {\n"
                  table.insert(stack,node)
                  table.insert(stack,v)
                  cache[node] = cur_index+1
                  break
              else
                  output_str = output_str .. string.rep('\t',depth) .. key .. " = '"..tostring(v).."'"
              end

              if (cur_index == size) then
                  output_str = output_str .. "\n" .. string.rep('\t',depth-1) .. "}"
              else
                  output_str = output_str .. ","
              end
          else
              -- close the table
              if (cur_index == size) then
                  output_str = output_str .. "\n" .. string.rep('\t',depth-1) .. "}"
              end
          end

          cur_index = cur_index + 1
      end

      if (size == 0) then
          output_str = output_str .. "\n" .. string.rep('\t',depth-1) .. "}"
      end

      if (#stack > 0) then
          node = stack[#stack]
          stack[#stack] = nil
          depth = cache[node] == nil and depth + 1 or depth - 1
      else
          break
      end
  end

  -- This is necessary for working with HUGE tables otherwise we run out of memory using concat on huge strings
  table.insert(output,output_str)
  output_str = table.concat(output)

  print(output_str)
end

local function format_any_value(obj, buffer)
  local _type = type(obj)
  if _type == "table" then
      buffer[#buffer + 1] = '{"'
      for key, value in next, obj, nil do
          buffer[#buffer + 1] = tostring(key) .. '":'
          format_any_value(value, buffer)
          buffer[#buffer + 1] = ',"'
      end
      buffer[#buffer] = '}' -- note the overwrite
  elseif _type == "string" then
      buffer[#buffer + 1] = '"' .. obj .. '"'
  elseif _type == "boolean" or _type == "number" then
      buffer[#buffer + 1] = tostring(obj)
  else
      buffer[#buffer + 1] = '"???' .. _type .. '???"'
  end
end

local function format_as_json(obj)
  if obj == nil then return "null" else
      local buffer = {}
      format_any_value(obj, buffer)
      return table.concat(buffer)
  end
end

local function print_as_json(obj)
  print(format_as_json(obj))
end

function dumpTable(table, depth)
  if not depth then depth = 0 end
  if (depth > 200) then
    print("Error: Depth > 200 in dumpTable()")
    return
  end
  for k,v in pairs(table) do
    if (type(v) == "table") then
      print(string.rep("  ", depth)..k..":")
      dumpTable(v, depth+1)
    else
      print(string.rep("  ", depth)..k..": ",v)
    end
  end
end