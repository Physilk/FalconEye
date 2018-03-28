local meshCubeTexturePath = "./data/debug2x2red.png"
local skyTexturePath = "./data/cubemap/church.jpg"
local batTexturePath = "./data/bat/bat/Bat_D.tga"

--orbiter
local origin = Point(0, 0, 0)
local myOrbiter = Orbiter(origin, 5)

--the shpere
local myTransform = Transform.Identity()

local myColor = Color(0.81, 0.01, 0.01, 1)
--local myMaterial = Material(myColor, 16, 0, 1)
local myMaterial = Material.make_textured_material(batTexturePath, 0, 0, 1)
local myPoint = Point(0, 0, 0)
local mySphere = Sphere(myTransform, myPoint, 1, myMaterial)

--the mesh
local meshObjectTransform = Transform.compose_transform(Transform.Scale(2, 2, 2), Transform.RotationY(0))
local meshObjectColor = Color(0.81, 0.5, 0.81, 1)
--local meshObjectMaterial = Material(meshObjectColor, 64, 0, 1)
local meshObjectMaterial = Material.make_textured_material(batTexturePath, 64, 0, 1)
local myMeshObject = MeshObject(meshObjectTransform, "data/bat/bat/bat2.obj", meshObjectMaterial)
--myMeshObject.smoothTriangles = true

--the cube (mesh)
local meshCubeTransform = Transform.compose_transform(Transform.Scale(2, 2, 2), Transform.compose_transform(Transform.RotationX(30), Transform.RotationY(30)))
local meshCubeMaterial = Material.make_textured_material(meshCubeTexturePath, 64, 0, 1)
--local meshCubeMaterial = meshObjectMaterial
local meshCube = MeshObject(meshCubeTransform, "data/cube.obj", meshCubeMaterial)

--the light
local lightColor = Color(0.95, 0.95, 0.95, 0)
local lightPos = Point(5, 7, 10)
local myPointLight = PointLight(lightPos, 100, lightColor)

--inverse sphere for the sky
local myInverseSphereColor = Color(0.6, 0.6, 0.8, 1)
--local myInverseSphereMaterial = Material(myInverseSphereColor, 0, 0, 1)
local myInverseSphereMaterial = Material.make_textured_material(skyTexturePath, 0, 0, 1)
local myInverseSphere = InverseSphere(myTransform, myPoint, 500, myInverseSphereMaterial)

--creation of the scene
local myScene = Scene(myOrbiter)
--myScene:addObject(mySphere)
myScene:addObject(myMeshObject)
--myScene:addObject(meshCube)
myScene:addObject(myInverseSphere)
myScene:addLight(myPointLight)

local sceneRenderOption = SceneRenderOption(800, 600, 60, 5, 1)
for i=0, -10, -1 do
	--render the scene
	myOrbiter:rotation(i/10, 0)
	myScene.orbiter = myOrbiter
	myScene:renderScene("output/test".. i .. ".png", sceneRenderOption)
end