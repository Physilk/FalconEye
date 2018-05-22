require 'data/lua/generated/enums'

local meshBoulePath = "./data/bodyparts/boule.obj"
local normalSampler = NormalSampler()

--orbiter
local origin = Point(0, 5, 30)
local myOrbiter = Orbiter(origin, 1)
--myOrbiter:rotation(180, 0)

--sampler
local cst_smpl_16 = ConstantScalarSampler(16);
local cst_smpl_0 = ConstantScalarSampler(0);
local cst_smpl_1 = ConstantScalarSampler(1);
local cst_smpl_0_3 = ConstantScalarSampler(0.3);

--planBottom
local plan1Color = ConstantColorSampler(Color(0.81, 0.01, 0.01, 1))
local plan1Material = Material(plan1Color, normalSampler, cst_smpl_16, cst_smpl_0, cst_smpl_1)
local plan1Point = Point(0, -10, 0)
local plan1Normal = Vector(0, 1, 0)
local plan1 = Plan(plan1Point, plan1Normal, plan1Material)
--planTop
local plan2Color = ConstantColorSampler(Color(0.01, 0.81, 0.01, 1))
local plan2Material = Material(plan2Color, normalSampler, cst_smpl_16, cst_smpl_0, cst_smpl_1)
local plan2Point = Point(0, 10, 0)
local plan2Normal = Vector(0, -1, 0)
local plan2 = Plan(plan2Point, plan2Normal, plan2Material)
--planLeft
local plan3Color = ConstantColorSampler(Color(0.01, 0.01, 0.81, 1))
local plan3Material = Material(plan3Color, normalSampler, cst_smpl_16, cst_smpl_0, cst_smpl_1)
local plan3Point = Point(-10, 0, 0)
local plan3Normal = Vector(1, 0, 0)
local plan3 = Plan(plan3Point, plan3Normal, plan3Material)
--planRight
local plan4Color = ConstantColorSampler(Color(0.81, 0.81, 0.01, 1))
local plan4Material = Material(plan4Color, normalSampler, cst_smpl_16, cst_smpl_0, cst_smpl_1)
local plan4Point = Point(10, 0, 0)
local plan4Normal = Vector(-1, 0, 0)
local plan4 = Plan(plan4Point, plan4Normal, plan4Material)
--planBack
local plan5Color = ConstantColorSampler(Color(0.01, 0.81, 0.81, 1))
local plan5Material = Material(plan5Color, normalSampler, cst_smpl_0, cst_smpl_0, cst_smpl_1)
local plan5Point = Point(0, 0, -50)
local plan5Normal = Vector(0, 0, 1)
local plan5 = Plan(plan5Point, plan5Normal, plan5Material)

--the shpere
local myColor = ConstantColorSampler(Color(0.81, 0.01, 0.81, 1))
local myMaterial = Material(myColor, normalSampler, cst_smpl_16, cst_smpl_0_3, cst_smpl_1)
local myPoint = Point(0, -10, -20)
local mySphere = Sphere(myPoint, 5, myMaterial)
local myBoule = MeshObject(Transform.compose_transform(Transform.Translation(Vector(0, -10, -20)), Transform.Scale(10, 10, 10)), meshBoulePath, myMaterial)

--the light
local lightColor = Color(0.95, 0.95, 0.95, 0)
local lightPos = Point(5, 7, -40)
local lightRange = 100
local myPointLight = createPointLigth(lightPos, lightRange, lightColor)

--creation of the scene
local myScene = Scene(myOrbiter)
--createChessBoard(myScene, 15, 15)
myScene:addObject(plan1)
myScene:addObject(plan2)
myScene:addObject(plan3)
myScene:addObject(plan4)
myScene:addObject(plan5)
myScene:addObject(mySphere)
--myScene:addObject(myBoule)
myScene:addLight(myPointLight)

local sceneRenderOption = SceneRenderOption(800, 600, 60, 5, 1)
for i=0, 20, 1 do
	--render the scene
    --myOrbiter:rotation(i, 0)
	--myScene.orbiter = myOrbiter
    lightPos = Point(lightPos.x, lightPos.y, lightPos.z + 5)
    myPointLight.position = lightPos
    --local p = mySphere.center
    --p.y = p.y + 1
    --mySphere.center = p
    --local radius = mySphere.radius + 0.2;
    --mySphere.radius = radius
	myScene:renderScene("output/plan/test_plan".. i .. ".png")
end

pauseUntilEnter()
