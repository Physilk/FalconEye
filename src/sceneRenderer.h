#pragma once


#include <random>
#include <chrono>

#include "luaObjectInterface.h"
#include "gKit/image.h"

namespace FalconEye {

	class Scene;
	struct Ray;
	struct Hit;
	
	namespace SceneRenderer {
		
    class SceneRenderOption {
    private:
        size_t width;
        size_t height;
        size_t fov;
        size_t reflection_bounce;
        


    public:
        SceneRenderOption() : width(1920),
            height(1080),
            fov(60),
            reflection_bounce(3)
        {}
        SceneRenderOption(size_t w, size_t h, size_t f, size_t b) :width(w),
            height(h),
            fov(f),
            reflection_bounce(b)
        {}
        void setWidth(size_t w) { width = w; }
        void setHeight(size_t h) { height = h; }
        void setFov(size_t f) { fov = f % 360; }
        void setReflectionBounce(size_t r) { reflection_bounce = r; }

        size_t getWidth() const { return width; }
        size_t getHeight() const { return height; }
        size_t getFov() const { return fov; }
        size_t getReflectionBounce() const { return reflection_bounce; }

        static SceneRenderOption defaultRenderOptions;

        LUA_BEGIN_BIND_METHODS(SceneRenderOption)
            LUA_BIND_CONSTRUCTOR_SP(SceneRenderOption, size_t, size_t, size_t, size_t)
            LUA_BIND_PROPERTY(SceneRenderOption, width, getWidth, setWidth)
            LUA_BIND_PROPERTY(SceneRenderOption, height, getHeight, setHeight)
            LUA_BIND_PROPERTY(SceneRenderOption, fov, getFov, setFov)
            LUA_BIND_PROPERTY(SceneRenderOption, ReflectionBounce, getReflectionBounce, setReflectionBounce)
            LUA_END_BIND_METHODS
    };

    using SceneRenderOption_ptr = std::shared_ptr<SceneRenderOption>;
    
    class Sampler {
	public:
		struct Sample {
			int x;
			int y;
			float fx;
			float fy;
		};
	protected:
		int x_start;
		int x_end;
		int y_start;
		int y_end;
		int sample_per_pixels;
	
	protected:
		Sampler() = delete;
		Sampler(const Sampler&) = default;
		Sampler(int xs, int xe, int ys, int ye, int spp)
		: x_start(xs)
		, x_end(xe)
		, y_start(ys)
		, y_end(ye)
		, sample_per_pixels(spp)
		{}
		
	public:
		virtual bool getNextSample(Sample &s) = 0;
		
		int totalSamples() { return sample_per_pixels * (x_end - x_start) * (y_end - y_start); }
	};
	
	class BasicRandomSampler : public Sampler {
	protected:
		int current_x;
		int current_y;
		int current_sample;
		
		std::default_random_engine random_engine;
	public:
		BasicRandomSampler() = delete;
		BasicRandomSampler(const BasicRandomSampler&) = default;
		BasicRandomSampler(int xs, int xe, int ys, int ye, int spp)
		: Sampler(xs, xe, ys, ye, spp)
		, current_x(xs)
		, current_y(ys)
		, current_sample(0)
		, random_engine(std::chrono::system_clock::now().time_since_epoch().count())
		{}
		
		virtual bool getNextSample(Sample &s);
	};
	
	Image renderScene(const Scene& s, const SceneRenderOption_ptr& ro = nullptr);
	bool castRay(const Scene& s, const Ray& r, Hit& h);
	bool castRay(const Scene& s, const Ray& r);
	Color shade(const Scene& s, const Ray& r, const Hit& h, int allowed_child_ray_depth = 0);
	 
	} // end namespace SceneRenderer

} // end namespace FalconEye
