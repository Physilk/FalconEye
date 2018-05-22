#pragma once

#include "luaObjectInterface.h"

#include "scene.h"
#include "material.h"
#include "gKit/color.h"
#include "gKit/vec.h"
#include "gKit/orbiter.h"
#include "gKit/mat.h"
#include "SceneObject.h"
#include "light.h"

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
            LUA_BIND_CLASS(Scene, L)
                LUA_BIND_CLASS(SceneRenderOption, L)
                LUA_BIND_CLASS(Material, L)
                LUA_BIND_CLASS(Color, L)
                LUA_BIND_CLASS(Point, L)
                LUA_BIND_CLASS(Vector, L)
                LUA_BIND_CLASS(vec2, L)
                LUA_BIND_CLASS(vec3, L)
                LUA_BIND_CLASS(vec4, L)
                LUA_BIND_CLASS(Orbiter, L)
                LUA_BIND_CLASS(Transform, L)
                LUA_BIND_CLASS(SceneObject, L)
                LUA_BIND_CLASS(Plan, L)
                LUA_BIND_CLASS(Sphere, L)
                LUA_BIND_CLASS(Triangle, L)
                LUA_BIND_CLASS(MeshObject, L)
                LUA_BIND_CLASS(InverseSphere, L)
                LUA_BIND_CLASS(PointLight, L)
                LUA_BIND_CLASS(PointLight::AttenuationParameters, L)
                LUA_BIND_CLASS(ColorSampler, L)
                LUA_BIND_CLASS(ScalarSampler, L)
                LUA_BIND_CLASS(NormalSampler, L)
                LUA_BIND_CLASS(ConstantColorSampler, L)
                LUA_BIND_CLASS(TextureColorSampler, L)
                LUA_BIND_CLASS(ConstantScalarSampler, L)
                LUA_BIND_CLASS(TextureNormalSampler, L)
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
