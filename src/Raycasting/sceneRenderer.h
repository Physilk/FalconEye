#pragma once


#include <random>
#include <chrono>

#include "LuaInterface/luaObjectInterface.h"
#include "gKit/image.h"
#include "gKit/vec.h"

#include "Threading/ThreadingInterface.h"
#include "Threading/JobBase.h"

#include "renderingContext.h"

using ::Threading::TJobBase;
using std::string;

namespace FalconEye {

	class Scene;
	struct Ray;
	struct Hit;
	
	class SceneRenderer {
	public:
	
		class SceneRenderOption {
		private:
			size_t width;
			size_t height;
			float fov;
			size_t reflection_bounce;
			size_t sample_per_pixels;
			string outputFilename;
			


		public:
			SceneRenderOption()
				: width(1920)
				, height(1080)
				, fov(60.0f)
				, reflection_bounce(3)
				, sample_per_pixels(1)
				, outputFilename()
			{}
			SceneRenderOption(size_t w, size_t h, float f, size_t b, size_t spp, string inOutputFilename)
				: width(w)
				, height(h)
				, fov(f)
				, reflection_bounce(b)
				, sample_per_pixels(spp > 0 ? spp : 1)
				, outputFilename(inOutputFilename)
			{}

			void setWidth(size_t w) { width = w; }
			void setHeight(size_t h) { height = h; }
			void setFov(float f) { fov = f; }
			void setReflectionBounce(size_t r) { reflection_bounce = r; }
			void setSamplerPerPixels(size_t spp) { sample_per_pixels = spp > 0 ? spp : 1; }
			void setOutputFileName(string o) { outputFilename = o; }

			size_t getWidth() const { return width; }
			size_t getHeight() const { return height; }
			float getFov() const { return fov; }
			size_t getReflectionBounce() const { return reflection_bounce; }
			size_t getSamplerPerPixels() const { return sample_per_pixels; }
			const string& getOutputFilename() const { return outputFilename; }

			static SceneRenderOption defaultRenderOptions;

			LUA_BEGIN_BIND_METHODS(SceneRenderOption)
				LUA_BIND_CONSTRUCTOR_SP(SceneRenderOption, size_t, size_t, float, size_t, size_t, string)
				LUA_BIND_PROPERTY(SceneRenderOption, width, getWidth, setWidth)
				LUA_BIND_PROPERTY(SceneRenderOption, height, getHeight, setHeight)
				LUA_BIND_PROPERTY(SceneRenderOption, fov, getFov, setFov)
				LUA_BIND_PROPERTY(SceneRenderOption, ReflectionBounce, getReflectionBounce, setReflectionBounce)
				LUA_BIND_PROPERTY(SceneRenderOption, SamplerPerPixels, getSamplerPerPixels, setSamplerPerPixels)
				LUA_BIND_PROPERTY(SceneRenderOption, outputFilename, getOutputFilename, setOutputFileName)
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
			virtual ~Sampler() = default;

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

			friend class AveragingJob;
		protected:
			// reference counting on threads should work but data race can occure (but we don't care as long as we dont modify the context)
			RenderingContext_ptr Context;
			Sampler* sampler;
			Image* output;
			SceneRenderOption renderOptions;
		public:
			RenderingJob() = delete;
			RenderingJob(const RenderingJob&) = default;
			RenderingJob(RenderingContext_ptr InContext, Sampler* smpl, Image* img, SceneRenderOption ro)
			: TJobBase()
			, Context(InContext)
			, sampler(smpl)
			, output(img)
			, renderOptions(ro)
			{}
			
			virtual ~RenderingJob() { delete sampler; }

		private:
			virtual int VirtualExecute();
		};
		
		using RenderingJob_ptr = std::shared_ptr<RenderingJob>;
		
		class AveragingJob : public TJobBase
		{
		protected:
			RenderingContext_ptr Context;
			std::vector<RenderingJob_ptr> RenderingJobs;
			Image* output;
			SceneRenderOption renderOptions;
		public:
			AveragingJob() = delete;
			AveragingJob(const AveragingJob&) = default;
			AveragingJob(RenderingContext_ptr InContext, Image* img, SceneRenderOption ro, const std::vector<RenderingJob_ptr>& inRenderingJobs)
			: TJobBase()
			, Context(InContext)
			, RenderingJobs(inRenderingJobs)
			, output(img)
			, renderOptions(ro)
			{}
			
			virtual ~AveragingJob() = default;

		private:
			virtual int VirtualExecute();
		};

		using AveragingJob_ptr = std::shared_ptr<AveragingJob>;
		//------------------------------------------------------------------
		//------------------------------------------------------------------


	private:
		RenderingContext_ptr Context = nullptr;

	public:
		SceneRenderer() {}

	public:
		void SetContext(RenderingContext_ptr InContext) { Context = InContext; };

		bool renderScene(Image* image, std::vector<Threading::TJob_ptr>& OutRenderingJobs, const SceneRenderOption_ptr& ro = nullptr);
		bool castRay(const Ray& r, Hit& h) const;
		bool castRay(const Ray& r) const;
		Color shade(const Ray& r, const Hit& h, int allowed_child_ray_depth = 0) const;

		//geometry functions
		static void createCoordinateSystem(const Vector& N, Vector& Nt, Vector& Nb);
		static Vector uniformSampleHemisphere(const float &r1, const float &r2); 
		static Vector rotateToCoordinateSystem(const Vector& v, const Vector& N, const Vector& Nt, const Vector& Nb);
	
	}; // end class SceneRenderer

	using SceneRenderer_ptr = std::shared_ptr<SceneRenderer>;
} // end namespace FalconEye
