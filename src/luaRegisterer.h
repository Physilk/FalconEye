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
        }
    };
}
