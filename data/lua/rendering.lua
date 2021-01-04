
local RenderingInterface_defaults = {
    --scene render options
    width = 800,
    height = 600,
    fov = 60,
    reflection_bounce = 2,
    sample_per_pixels = 1,
	output_file,

    --orbiter
    center = FalconEye.Point(0, 0, 0),
    distance = 5,

    --lights
    --attenuation parameters
    constant = 1,
    linear =  0, 
    quadratic = 0,
    --point light
    position = FalconEye.Point(0, 0 , 0),
    range = 100,
    color = FalconEye.Color(1, 1, 1, 1),
    --sphere light
    radius = 1,
	arg_nbSamples = 64,
}

local function make_scene_render_options(args)
	local arg_width = args.width
	local arg_height = args.height
	local arg_fov = args.fov
	local arg_reflection_bounce = args.reflection_bounce
    local arg_sample_per_pixels = args.sample_per_pixels
	local arg_output_file = args.output_file

	if not arg_width then 
		arg_color = RenderingInterface_defaults.width
	end
	if not arg_height then
		arg_shininess = RenderingInterface_defaults.height
	end
	if not arg_fov then
		arg_reflectivity = RenderingInterface_defaults.fov
	end
	if not arg_reflection_bounce then
		arg_refraction = RenderingInterface_defaults.reflection_bounce
	end
	if not arg_sample_per_pixels then
		arg_refraction = RenderingInterface_defaults.sample_per_pixels
	end
	if not arg_output_file then
		arg_output_file = RenderingInterface_defaults.output_file
	end
	return FalconEye.RayCasting.SceneRenderOption(arg_width, arg_height, arg_fov, arg_reflection_bounce, arg_sample_per_pixels, arg_output_file)
end

--lights
local function make_attenuation_parameters(args)
    local arg_constant = args.constant
	local arg_linear = args.linear
	local arg_quadratic = args.quadratic

    if not arg_constant then 
		arg_constant = RenderingInterface_defaults.constant
	end
	if not arg_linear then
		arg_linear = RenderingInterface_defaults.linear
	end
	if not arg_quadratic then
		arg_quadratic = RenderingInterface_defaults.quadratic
	end

    return FalconEye.RayCasting.AttenuationParameters(arg_constant, arg_linear, arg_quadratic)
end

local function make_attenuation_from_range(range)
    return FalconEye.RayCasting.AttenuationParameters(1, 4.5/range, 75/(range*range))
end

local function make_point_light(args)
    local arg_position = args.position
	local arg_range = args.range
	local arg_color = args.color
    local arg_attenuation = args.attenuation

    if not arg_position then 
		arg_position = RenderingInterface_defaults.position
	end
	if not arg_range then
		arg_range = RenderingInterface_defaults.range
	end
	if not arg_color then
		arg_color = RenderingInterface_defaults.color
	end
    if not arg_attenuation then
		arg_attenuation = make_attenuation_from_range(arg_range)
	end

    return FalconEye.RayCasting.PointLight(arg_position, arg_range, arg_color, arg_attenuation)
end

local function make_sphere_light(args)
    local arg_position = args.position
	local arg_range = args.range
	local arg_radius = args.radius
	local arg_nbSamples = args.nbSamples
	local arg_color = args.color
    local arg_attenuation = args.attenuation

    if not arg_position then 
		arg_position = RenderingInterface_defaults.position
	end
	if not arg_range then
		arg_range = RenderingInterface_defaults.range
	end
	if not arg_radius then
		arg_radius = RenderingInterface_defaults.radius
	end
	if not arg_nbSamples then
		arg_nbSamples = RenderingInterface_defaults.arg_nbSamples
	end
	if not arg_color then
		arg_color = RenderingInterface_defaults.color
	end
    if not arg_attenuation then
		arg_attenuation = make_attenuation_from_range(arg_range)
	end

    return FalconEye.RayCasting.SphereLight(arg_position, arg_range, arg_radius, arg_nbSamples, arg_color, arg_attenuation)
end


--orbiter
local function make_orbiter(args)
    local arg_center = args.center
    local arg_distance = args.distance

    if not arg_center then 
		arg_center = RenderingInterface_defaults.center
	end
	if not arg_distance then
		arg_distance = RenderingInterface_defaults.distance
	end
    
    return FalconEye.Orbiter(arg_center, arg_distance)
end

--scene
local function make_scene(args)
    local arg_orbiter = args.orbiter
    local arg_objects = args.objects
    local arg_lights = args.lights
    
    if not arg_orbiter then 
		arg_orbiter = make_orbiter{}
	end

    local scene = FalconEye.RayCasting.Scene(arg_orbiter)

    if arg_objects then 
        for i, obj in ipairs(arg_objects) do
            scene:addObject(obj)
        end
	end
	if arg_lights then
		for i, ligth in ipairs(arg_lights) do
            scene:addLight(ligth)
        end
	end
    return scene
end

RenderingInterface = {
	make_scene_render_options = make_scene_render_options,
    make_attenuation_parameters = make_attenuation_parameters,
    make_attenuation_from_range = make_attenuation_from_range,
    make_point_light = make_point_light,
    make_sphere_light = make_sphere_light,
    make_orbiter = make_orbiter,
    make_scene = make_scene,
}

return RenderingInterface
