#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "luaObjectInterface.h"
#include "orbiter.h"
#include "ray.h"
#include "SceneObject.h"
#include "light.h"
#include "sceneRenderer.h"

using std::vector;
using LuaIntf::_def;

namespace FalconEye {

    //forward declaration
    class BBoxBinTree;
    class SceneObject;
    class PointLight;
    struct Hit;
    struct Ray;

    class Scene {
    private:
        vector<SceneObject_ptr> objects;
        vector<PointLight_ptr> lights;

        float shadow_coeficient = 0.2f;

        Orbiter camera;

        BBoxBinTree * bboxBinTree;

        Point boundMin;
        Point boundMax;

        void initBounds();

        bool intersect(const Ray&, Hit&, const SceneObject* ignore = nullptr);
        bool intersect(const Ray&, const SceneObject* ignore = nullptr);
        Color processPixelColor(const Ray &, const Hit&, const size_t possible_reflection_bounce = 0);

    public:
        Scene() : objects(), lights(), camera(Orbiter()), bboxBinTree(nullptr) { initBounds(); }
        Scene(Orbiter o) : objects(), lights(), camera(o), bboxBinTree(nullptr) { initBounds(); }

        ~Scene();

        void addObject(SceneObject_ptr o);
        void addLight(PointLight_ptr l) { lights.push_back(l); }

        void preProcess();
        void renderScene(const char *filename, SceneRenderOption_ptr opt = nullptr);

        size_t getCount();

        Orbiter getOrbiter() { return camera; }
        void setOrbiter(Orbiter o) { camera = o; }

        LUA_BEGIN_BIND_METHODS(Scene)
            LUA_BIND_CONSTRUCTOR_NOARGS
            LUA_BIND_CONSTRUCTOR(Orbiter)
            LUA_BIND_METHOD(Scene, addObject)
            LUA_BIND_METHOD(Scene, addLight)
            LUA_BIND_METHOD(Scene, preProcess)
            //LUA_BIND_METHOD(Scene, renderScene)
            LUA_BIND_METHOD_ARGS(Scene, renderScene, const char *, _opt<SceneRenderOption_ptr>)
            LUA_BIND_PROPERTY(Scene, orbiter, getOrbiter, setOrbiter)
            LUA_END_BIND_METHODS
    };
}

#endif
