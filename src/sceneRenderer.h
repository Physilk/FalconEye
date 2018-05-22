#pragma once

#include "luaObjectInterface.h"

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

        static SceneRenderOption defaultRenderOptions;

        LUA_BEGIN_BIND_METHODS(SceneRenderOption)
            LUA_BIND_CONSTRUCTOR_SP(SceneRenderOption, size_t, size_t, size_t, size_t, size_t)
            LUA_BIND_PROPERTY(SceneRenderOption, width, getWidth, setWidth)
            LUA_BIND_PROPERTY(SceneRenderOption, height, getHeight, setHeight)
            LUA_BIND_PROPERTY(SceneRenderOption, fov, getFov, setFov)
            LUA_BIND_PROPERTY(SceneRenderOption, ReflectionBounce, getReflectionBounce, setReflectionBounce)
            LUA_BIND_PROPERTY(SceneRenderOption, AntiAliasing, getAntiAliasing, setAntiAliasing)
            LUA_END_BIND_METHODS
    };

    using SceneRenderOption_ptr = std::shared_ptr<SceneRenderOption>;

} // end namespace FalconEye
