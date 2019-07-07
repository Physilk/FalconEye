

local MaterialInterface_sampler_arrays = {
	cst_color_samplers = {},
	--cst_normal_samplers = {},
	cst_scalar_samplers = {},
	txtr_color_sampler = {},
}

local function getConstantScalarSampler(cst)
	if MaterialInterface_sampler_arrays.cst_scalar_samplers[cst] == nil then
		MaterialInterface_sampler_arrays.cst_scalar_samplers[cst] = FalconEye.RayCasting.ConstantScalarSampler(cst)
	end
	return MaterialInterface_sampler_arrays.cst_scalar_samplers[cst]
end

local function getConstantColorSampler(color)
	local index = colorToString(color)
	if MaterialInterface_sampler_arrays.cst_color_samplers[index] == nil then
		MaterialInterface_sampler_arrays.cst_color_samplers[index] = FalconEye.RayCasting.ConstantColorSampler(color)
	end
	return MaterialInterface_sampler_arrays.cst_color_samplers[index]
end

local function getTextureColorSampler(texture_file_name)
	if MaterialInterface_sampler_arrays.txtr_color_sampler[texture_file_name] == nil then
		MaterialInterface_sampler_arrays.txtr_color_sampler[texture_file_name] = FalconEye.RayCasting.TextureColorSampler(texture_file_name)
	end
	return MaterialInterface_sampler_arrays.txtr_color_sampler[texture_file_name]
end

--local function getConstantNormalSampler(cst)
--	if MaterialInterface_constant_sampler_arrays.normal_samplers[cst] == nil then
--		MaterialInterface_constant_sampler_arrays.normal_samplers[cst] = NormalSampler()
--	end
--	return MaterialInterface_constant_sampler_arrays.normal_samplers[cst]
--end

local MaterialInterface_defaults = {
	albedo_sampler = getConstantColorSampler(FalconEye.Color(0.75, 0.75, 0.75, 1.0)),
	normal_sampler = FalconEye.RayCasting.NormalSampler(),
	shininess_sampler = getConstantScalarSampler(16),
	reflectivity_sampler = getConstantScalarSampler(0),
	refraction_sampler = getConstantScalarSampler(1),
}

local function make_material(args)
	local arg_color = args.albedo
	local arg_shininess = args.specular_exponent
	local arg_reflectivity = args.reflectivity
	local arg_refraction = args.refraction
	
	if arg_color and not args.albedo_texture then
		arg_color = getConstantColorSampler(arg_color)
	end
	
	if arg_shininess and not args.specular_exponent_texture then
		arg_shininess = getConstantScalarSampler(arg_shininess)
	end
	
	if arg_reflectivity and not args.reflectivity_texture then
		arg_reflectivity = getConstantScalarSampler(arg_reflectivity)
	end
	
	if arg_refraction and not args.refraction_texture then
		arg_refraction = getConstantScalarSampler(arg_refraction)
	end

	if args.albedo_texture then
		if type(args.albedo_texture) ~= "string" then
			error("argument albedo_texture must be a string")
		end
		arg_color = getTextureColorSampler(args.albedo_texture)
	end
	
	--if args.specular_exponent_texture then
	--	if type(args.specular_exponent) ~= "string" then
	--		error("argument specular_exponent_texture must be a string")
	--	end
	--	arg_shininess = getTextureColorSampler(args.specular_exponent_texture)
	--end
	
	--if args.reflectivity_texture then
	--	if type(args.reflectivity_texture) ~= "string" then
	--		error("argument reflectivity_texture must be a string")
	--end
	--	arg_reflectivity = getTextureColorSampler(args.reflectivity_texture)
	--end
	
	--if args.refraction_texture then
	--	if type(args.albedo_texture) ~= "string" then
	--		error("argument refraction_texture must be a string")
	--	end
	--	arg_refraction = getTextureColorSampler(args.refraction_texture)
	--end
	
	if not arg_color then 
		arg_color = MaterialInterface_defaults.albedo_sampler
	end
	if not arg_shininess then
		arg_shininess = MaterialInterface_defaults.shininess_sampler
	end
	if not arg_reflectivity then
		arg_reflectivity = MaterialInterface_defaults.reflectivity_sampler
	end
	if not arg_refraction then
		arg_refraction = MaterialInterface_defaults.refraction_sampler
	end
	return FalconEye.RayCasting.Material(arg_color, FalconEye.RayCasting.NormalSampler(), arg_shininess, arg_reflectivity, arg_refraction)
end

local function debug_nb_sampler()
	print("nb cst_color_samplers= " .. tablelength(MaterialInterface_sampler_arrays.cst_color_samplers))
	print("nb cst_scalar_samplers= " .. tablelength(MaterialInterface_sampler_arrays.cst_scalar_samplers))
	print("nb txtr_color_sampler= " .. tablelength(MaterialInterface_sampler_arrays.txtr_color_sampler))
end

MaterialInterface = {
	make_material = make_material,
	debug_print = debug_nb_sampler
}

return MaterialInterface
