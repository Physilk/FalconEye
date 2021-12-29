require 'data/lua/generated/enums'
local Mat = require 'data/lua/material'
local Rendering = require 'data/lua/rendering'
local Objects = require 'data/lua/objects'

local meshBoulePath = "./data/bodyparts/boule.obj"
local meshCubePath = "./data/cube.obj"

  
--orbiter
local origin = FalconEye.Point(0, 5, 20)
local myOrbiter = Rendering.make_orbiter{center = origin, distance = 10}
--myOrbiter:rotation(180, 0)


--planBottom
local plan1Material = Mat.make_material{albedo = FalconEye.Color(0.81, 0.01, 0.01, 1), specular_exponent = 16, reflectivity = 0, refraction = 1}
local plan1Point = FalconEye.Point(0, -10, 0)
local plan1Normal = FalconEye.Vector(0, 1, 0)
local plan1 = Objects.make_plan{material = plan1Material, position = plan1Point, normal = plan1Normal}
--planTop
local plan2Material = Mat.make_material{albedo = FalconEye.Color(0.01, 0.81, 0.01, 1), specular_exponent = 16, reflectivity = 0, refraction = 1}
local plan2Point = FalconEye.Point(0, 10, 0)
local plan2Normal = FalconEye.Vector(0, -1, 0)
local plan2 = Objects.make_plan{material = plan2Material, position = plan2Point, normal = plan2Normal}
--planLeft
local plan3Material = Mat.make_material{albedo = FalconEye.Color(0.01, 0.01, 0.81, 1), specular_exponent = 16, reflectivity = 0, refraction = 1}
local plan3Point = FalconEye.Point(-10, 0, 0)
local plan3Normal = FalconEye.Vector(1, 0, 0)
local plan3 = Objects.make_plan{material = plan3Material, position = plan3Point, normal = plan3Normal}
--planRight
local plan4Material = Mat.make_material{albedo = FalconEye.Color(0.81, 0.81, 0.01, 1), specular_exponent = 16, reflectivity = 0, refraction = 1}
local plan4Point = FalconEye.Point(10, 0, 0)
local plan4Normal = FalconEye.Vector(-1, 0, 0)
local plan4 = Objects.make_plan{material = plan4Material, position = plan4Point, normal = plan4Normal}
--planBack
local plan5Material = Mat.make_material{albedo = FalconEye.Color(0.01, 0.81, 0.81, 1), specular_exponent = 16, reflectivity = 0, refraction = 1}
local plan5Point = FalconEye.Point(0, 0, -50)
local plan5Normal = FalconEye.Vector(0, 0, 1)
local plan5 = Objects.make_plan{material = plan5Material, position = plan5Point, normal = plan5Normal}

--the shpere
local myMaterial = Mat.make_material{albedo = FalconEye.Color(0.81, 0.81, 0.81, 1), specular_exponent = 0, reflectivity = 0.0, refraction = 1}
--local myPoint = FalconEye.Point(0, -10, -20)
local myPoint = FalconEye.Point(0, 0, 0)
local mySphere = Objects.make_sphere{position = myPoint, radius = 5, material = myMaterial}--Sphere(myPoint, 5, myMaterial)
local myBoule = Objects.make_mesh_object{transform = FalconEye.Transform.compose_transform(FalconEye.Transform.Scale(100, 100, 100), FalconEye.Transform.Translation(FalconEye.Vector(0, 0, 0))), file_path = meshBoulePath, material = myMaterial}
local myCube = Objects.make_mesh_object{transform = FalconEye.Transform.compose_transform(FalconEye.Transform.compose_transform(FalconEye.Transform.Translation(FalconEye.Vector(0, -10, -20)), FalconEye.Transform.Scale(20, 20, 20)), FalconEye.Transform.RotationY(45)), file_path = meshCubePath, material = myMaterial}

--the shpere
local mySkyMaterial = Mat.make_material{albedo = FalconEye.Color(0.2, 0.2, 0.95, 1), specular_exponent = 16, reflectivity = 0.0, refraction = 1}
local myInvSphere = Objects.make_inverse_sphere{position = myPoint, radius = 100, material = mySkyMaterial}

--the light
local lightColor = FalconEye.Color(1.00, 1.00, 1.00, 0)
local lightPos = FalconEye.Point(5, 7, 20)
local lightRange = 1000
local myPointLight = Rendering.make_point_light{ position = lightPos, range = lightRange, color = lightColor} --createPointLigth(lightPos, lightRange, lightColor)

local directionalLight = Rendering.make_directional_light{ direction = FalconEye.Vector(-1, -1, 0), color = lightColor}

--the sphere light
local lightRadius = 1;
local mySphereLight = Rendering.make_sphere_light{ position = lightPos, range = lightRange, radius = lightRadius, nbSamples = 8, color = lightColor}

local scene = Rendering.make_scene{
    orbiter = myOrbiter,
    --objects = {plan1, plan2, plan3, plan4, plan5, mySphere, myInvSphere},
    objects = {mySphere, plan1, plan3},
    --lights = {mySphereLight}
    lights = {directionalLight}
}
scene:preProcess();

local mat_tile_dark = Mat.make_material{albedo = FalconEye.Color(0.1, 0.1, 0.1, 1), specular_exponent = 32, reflectivity = 0.5, refraction = 1}
local mat_tile_light = Mat.make_material{albedo = FalconEye.Color(0.8, 0.8, 0.8, 1), specular_exponent = 32, reflectivity = 0.5, refraction = 1}

--createChessBoard(myScene, 5, 5, 5, mat_tile_dark, mat_tile_light);

--Mat.debug_print()
local sceneRenderOption = Rendering.make_scene_render_options{width = 800, height = 600, fov = 60, reflection_bounce = 2, sample_per_pixels = 1, output_file = "output/test_plan.png"}
--for i=0, 5, 1 do
	--render the scene
    --myOrbiter:rotation(i, 0)
	--myScene.orbiter = myOrbiter
    ----lightPos = FalconEye.Point(lightPos.x, lightPos.y, lightPos.z + 5)
    ----myPointLight.position = lightPos
    --local p = mySphere.center
    --p.y = p.y + 1
    --mySphere.center = p
    --local radius = mySphere.radius + 0.2;
    --mySphere.radius = radius
	--myScene:renderScene("output/plan/test_plan".. i .. ".png", sceneRenderOption)
--myScene:renderScene("output/plan/mini_test_plan.png", sceneRenderOption)
--scene:renderScene("output/plan/mini_test_plan.png", sceneRenderOption)
--end

print_table(getmetatable(plan2))


-- App
local renderingApp = FalconEye.RayCasting.RaycastRenderingApp(900, 600, scene, 3, 3)
renderingApp.renderOption = sceneRenderOption
renderingApp:LuaRun()
--renderingApp:DebugRender()
--pauseUntilEnter()
