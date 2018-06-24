
local RenderingInterface_defaults = {
    width = 800,
    height = 600,
    fov = 60,
    reflection_bounce = 2,
    sample_per_pixels = 1,
}

local function make_scene_render_options(args)
	local arg_width = args.width
	local arg_height = args.height
	local arg_fov = args.fov
	local arg_reflection_bounce = args.reflection_bounce
    local arg_sample_per_pixels = args.sample_per_pixels
	
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
	return SceneRenderOption(arg_width, arg_height, arg_fov, arg_reflection_bounce, arg_sample_per_pixels)
end


RenderingInterface = {
	make_scene_render_options = make_scene_render_options,
}

return RenderingInterface
