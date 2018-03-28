#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "luaObjectInterface.h"
#include "orbiter.h"
#include "ray.h"
#include "SceneObject.h"
#include "light.h"

using std::vector;

namespace FalconEye {

    class SceneRenderOption {
    private:
        size_t width;
        size_t height;
        size_t fov;
        size_t reflection_bounce;
        size_t anti_aliasing;


    public:
        SceneRenderOption() : width(1920),
            height(1080),
            fov(60),
            reflection_bounce(3),
            anti_aliasing(1)
        {}
        SceneRenderOption(size_t w, size_t h, size_t f, size_t b, size_t aa) :width(w),
            height(h),
            fov(f),
            reflection_bounce(b),
            anti_aliasing(aa)
        {}
        void setWidth(size_t w) { width = w; }
        void setHeight(size_t h) { height = h; }
        void setFov(size_t f) { fov = f % 360; }
        void setReflectionBounce(size_t r) { reflection_bounce = r; }
        void setAntiAliasing(size_t aa) { (aa == 1 || aa == 2 || aa == 4 || aa == 8 || aa == 16) ? anti_aliasing = aa : anti_aliasing = 1; }

        size_t getWidth() const { return width; }
        size_t getHeight() const { return height; }
        size_t getFov() const { return fov; }
        size_t getReflectionBounce() const { return reflection_bounce; }
        size_t getAntiAliasing() const { return anti_aliasing; }

        LUA_BEGIN_BIND_METHODS(SceneRenderOption)
            LUA_BIND_CONSTRUCTOR_NOARGS
            LUA_BIND_CONSTRUCTOR(size_t, size_t, size_t, size_t, size_t)
            LUA_BIND_PROPERTY(SceneRenderOption, width, getWidth, setWidth)
            LUA_BIND_PROPERTY(SceneRenderOption, height, getHeight, setHeight)
            LUA_BIND_PROPERTY(SceneRenderOption, fov, getFov, setFov)
            LUA_BIND_PROPERTY(SceneRenderOption, ReflectionBounce, getReflectionBounce, setReflectionBounce)
            LUA_BIND_PROPERTY(SceneRenderOption, AntiAliasing, getAntiAliasing, setAntiAliasing)
            LUA_END_BIND_METHODS
    };

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
        void renderScene(const char *filename, const SceneRenderOption &opt);

        size_t getCount();

        Orbiter getOrbiter() { return camera; }
        void setOrbiter(Orbiter o) { camera = o; }

        LUA_BEGIN_BIND_METHODS(Scene)
            LUA_BIND_CONSTRUCTOR_NOARGS
            LUA_BIND_CONSTRUCTOR(Orbiter)
            LUA_BIND_METHOD(Scene, addObject)
            LUA_BIND_METHOD(Scene, addLight)
            LUA_BIND_METHOD(Scene, preProcess)
            LUA_BIND_METHOD(Scene, renderScene)
            LUA_BIND_PROPERTY(Scene, orbiter, getOrbiter, setOrbiter)
            LUA_END_BIND_METHODS
    };
}

#endif
