#pragma once


#include <random>
#include <chrono>

#include "luaObjectInterface.h"
#include "gKit/image.h"
#include "gKit/vec.h"

#include "ThreadingInterface.h"
#include "Threading/JobBase.h"

using ::Threading::TJobBase;

namespace FalconEye {

	class Scene;
	struct Ray;
	struct Hit;
	
	namespace SceneRenderer {
		
    class SceneRenderOption {
    private:
        size_t width;
        size_t height;
        float fov;
        size_t reflection_bounce;
        size_t sample_per_pixels;
        


    public:
        SceneRenderOption()
            : width(1920)
            , height(1080)
            , fov(60.0f)
            , reflection_bounce(3)
            , sample_per_pixels(1)
        {}
        SceneRenderOption(size_t w, size_t h, float f, size_t b, size_t spp)
            : width(w)
            , height(h)
            , fov(f)
            , reflection_bounce(b)
            , sample_per_pixels(spp)
        {}
        void setWidth(size_t w) { width = w; }
        void setHeight(size_t h) { height = h; }
        void setFov(float f) { fov = f; }
        void setReflectionBounce(size_t r) { reflection_bounce = r; }
        void setSamplerPerPixels(size_t spp) { sample_per_pixels = spp; }

        size_t getWidth() const { return width; }
        size_t getHeight() const { return height; }
        float getFov() const { return fov; }
        size_t getReflectionBounce() const { return reflection_bounce; }
        size_t getSamplerPerPixels() const { return sample_per_pixels; }

        static SceneRenderOption defaultRenderOptions;

        LUA_BEGIN_BIND_METHODS(SceneRenderOption)
            LUA_BIND_CONSTRUCTOR_SP(SceneRenderOption, size_t, size_t, float, size_t, size_t)
            LUA_BIND_PROPERTY(SceneRenderOption, width, getWidth, setWidth)
            LUA_BIND_PROPERTY(SceneRenderOption, height, getHeight, setHeight)
            LUA_BIND_PROPERTY(SceneRenderOption, fov, getFov, setFov)
            LUA_BIND_PROPERTY(SceneRenderOption, ReflectionBounce, getReflectionBounce, setReflectionBounce)
            LUA_BIND_PROPERTY(SceneRenderOption, SamplerPerPixels, getSamplerPerPixels, setSamplerPerPixels)
            LUA_END_BIND_METHODS
    };

    using SceneRenderOption_ptr = std::shared_ptr<SceneRenderOption>;
    
    //------------------------------------------------------------------
    //------------------------------------------------------------------
    
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
	
	//------------------------------------------------------------------
	
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
	
	//------------------------------------------------------------------
	//------------------------------------------------------------------
	
	class RenderingJob : public TJobBase {
	protected:
		const Scene* scene;
		Sampler* sampler;
		Image* output;
		const SceneRenderOption* renderOptions;
	public:
		RenderingJob() = delete;
		RenderingJob(const RenderingJob&) = default;
		RenderingJob(const Scene* sc, Sampler* smpl, Image* img, const SceneRenderOption* ro)
		: TJobBase()
		, scene(sc)
		, sampler(smpl)
		, output(img)
		, renderOptions(ro)
		{}
		
	private:
		virtual int VirtualExecute();
	};
	
	using RenderingJob_ptr = std::shared_ptr<RenderingJob>;
	
	//------------------------------------------------------------------
	//------------------------------------------------------------------
	
	Image renderScene(const Scene& s, const SceneRenderOption_ptr& ro = nullptr);
	bool castRay(const Scene& s, const Ray& r, Hit& h);
	bool castRay(const Scene& s, const Ray& r);
	Color shade(const Scene& s, const Ray& r, const Hit& h, int allowed_child_ray_depth = 0);
	 
	//geometry functions
	void createCoordinateSystem(const Vector& N, Vector& Nt, Vector& Nb);
	Vector uniformSampleHemisphere(const float &r1, const float &r2); 
	Vector rotateToCoordinateSystem(const Vector& v, const Vector& N, const Vector& Nt, const Vector& Nb);
	
	} // end namespace SceneRenderer

} // end namespace FalconEye
