#pragma once

#include "luaObjectInterface.h"

#include "Raycasting/scene.h"
#include "Raycasting/material.h"
#include "gKit/color.h"
#include "gKit/vec.h"
#include "gKit/orbiter.h"
#include "gKit/mat.h"
#include "Raycasting/SceneObject.h"
#include "Raycasting/light.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using std::ofstream;
using std::stringstream;
using std::string;

namespace FalconEye {
    class LUARegisterer
    {
    public:
        LUARegisterer() = delete;
        LUARegisterer(const LUARegisterer &) = delete;


        static inline void RegisterClasses(lua_State *L)
        {
			auto FalconEyeModule = LUA_START_BIND_MODULE_BASE(FalconEye, L);
			LUA_BIND_CLASS(Color, FalconEyeModule);
			LUA_BIND_CLASS(Point, FalconEyeModule);
			LUA_BIND_CLASS(Vector, FalconEyeModule);
			LUA_BIND_CLASS(vec2, FalconEyeModule);
			LUA_BIND_CLASS(vec3, FalconEyeModule);
			LUA_BIND_CLASS(vec4, FalconEyeModule);
			LUA_BIND_CLASS(Orbiter, FalconEyeModule);
			LUA_BIND_CLASS(Transform, FalconEyeModule);

			auto RayCastingModule = LUA_START_BIND_MODULE(RayCasting, FalconEyeModule);
			LUA_BIND_CLASS(Scene, RayCastingModule);
			LUA_BIND_CLASS(SceneRenderOption, RayCastingModule);
			LUA_BIND_CLASS(Material, RayCastingModule);
			LUA_BIND_CLASS(SceneObject, RayCastingModule);
			LUA_BIND_CLASS(Plan, RayCastingModule);
			LUA_BIND_CLASS(Sphere, RayCastingModule);
			LUA_BIND_CLASS(Triangle, RayCastingModule);
			LUA_BIND_CLASS(MeshObject, RayCastingModule);
			LUA_BIND_CLASS(InverseSphere, RayCastingModule);
			LUA_BIND_CLASS(Light, RayCastingModule);
			LUA_BIND_CLASS(PointLight, RayCastingModule);
			LUA_BIND_CLASS(SphereLight, RayCastingModule);
			LUA_BIND_CLASS(AttenuationParameters, RayCastingModule);
			LUA_BIND_CLASS(ColorSampler, RayCastingModule);
			LUA_BIND_CLASS(ScalarSampler, RayCastingModule);
			LUA_BIND_CLASS(NormalSampler, RayCastingModule);
			LUA_BIND_CLASS(ConstantColorSampler, RayCastingModule);
			LUA_BIND_CLASS(TextureColorSampler, RayCastingModule);
			LUA_BIND_CLASS(ConstantScalarSampler, RayCastingModule);
			LUA_BIND_CLASS(TextureNormalSampler, RayCastingModule);
			LUA_END_BIND_MODULE(RayCastingModule);

			LUA_END_BIND_MODULE(FalconEyeModule);
				//LUA_END_BIND_MODULE();
        }
        
        static inline void GenerateEnums(string generatedEnumFilePath = "./data/lua/generated/enums.lua")
        {
            ofstream file = ofstream(generatedEnumFilePath, std::ios::out | std::ios::trunc);
            if (file.is_open())
            {
				stringstream ss;
				LUA_ENUM_WRITE_ENUM(TextureChannel, ss)
				LUA_ENUM_WRITE_ENUM(TextureType, ss)
				
				file << ss.str();
				file.close();
			} else {
				std::cerr << "Warning: could not write generated enum to file" << generatedEnumFilePath << '\n';
			}
        }
    };
}
