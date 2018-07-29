local Mat = require 'data/lua/material'

local ObjectsInterface_defaults = {
    material = Mat.make_material{albedo = Color(0.81, 0.81, 0.81, 1), specular_exponent = 16, reflectivity = 0, refraction = 1},

    position = Point(0, 0, 0),
    normal = Vector(0, 1, 0),

    radius = 1,

    transform = Transform.Identity()
}

local function make_plan(args)
    local arg_position = args.position
    local arg_normal = args.normal
    local arg_material = args.material
    
    if not arg_position then 
		arg_position = ObjectsInterface_defaults.position
	end
    if not arg_normal then
        arg_normal = ObjectsInterface_defaults.normal
	end
	if not arg_material then
        arg_material = ObjectsInterface_defaults.material
	end
    return Plan(arg_position, arg_normal, arg_material)
end

local function make_sphere(args)
    local arg_position = args.position
    local arg_radius = args.radius
    local arg_material = args.material

    if not arg_position then 
		arg_position = ObjectsInterface_defaults.position
	end
    if not arg_radius then
        arg_radius = ObjectsInterface_defaults.radius
	end
	if not arg_material then
        arg_material = ObjectsInterface_defaults.material
	end
    return Sphere(arg_position, arg_radius, arg_material)
end

local function make_inverse_sphere(args)
    local arg_position = args.position
    local arg_radius = args.radius
    local arg_material = args.material

    if not arg_position then 
		arg_position = ObjectsInterface_defaults.position
	end
    if not arg_radius then
        arg_radius = ObjectsInterface_defaults.radius
	end
	if not arg_material then
        arg_material = ObjectsInterface_defaults.material
	end
    return InverseSphere(arg_position, arg_radius, arg_material) 
end

local function make_triangle(args)
    local arg_v0 = args.v0
    local arg_v1 = args.v1
    local arg_v2 = args.v2
    local arg_material = args.material

    if not arg_v0 then 
		error("v0 not specified")
	end
    if not arg_v1 then
        error("v1 not specified")
	end
	if not arg_v2 then
        error("v2 not specified")
	end
    if not arg_material then
        arg_material = ObjectsInterface_defaults.material
	end
    return Triangle(arg_v0, arg_v1, arg_v2, arg_material) 
end

local function make_mesh_object(args)
    local arg_transform = args.transform
    local arg_file_path = args.file_path
    local arg_material = args.material

    if not arg_transform then 
		arg_transform = ObjectsInterface_defaults.transform
	end
    if type(arg_file_path) ~= "string" then
        error("file_path not specified or is not a string")
	end
    if not arg_material then
        arg_material = ObjectsInterface_defaults.material
	end
    return MeshObject(arg_transform, arg_file_path, arg_material) 
end

ObjectsInterface = {
    make_plan = make_plan,
    make_sphere = make_sphere,
    make_inverse_sphere = make_inverse_sphere,
    make_triangle = make_triangle,
    make_mesh_object = make_mesh_object,

}

return ObjectsInterface
