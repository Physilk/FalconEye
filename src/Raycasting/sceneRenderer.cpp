#include "sceneRenderer.h"
#include "scene.h"
#include "ray.h"
#include "bintree.h"
#include "Utils/tools.h"
#include "material.h"


using Threading::TThreadManager;

namespace FalconEye {
		
    SceneRenderOption SceneRenderOption::defaultRenderOptions = SceneRenderOption();
    
    bool SceneRenderer::BasicRandomSampler::getNextSample(Sample &s)
    {
		if (current_sample > sample_per_pixels)
		{
			current_sample = 0;
			++current_x;
			if (current_x >= x_end)
			{
				current_x = x_start;
				++current_y;
				if (current_y >= y_end)
				{
					return false;
				}
			}
		}
		
		s.x = current_x;
		s.y = current_y;
		
        float r1 = random_engine();
        float r2 = random_engine();
		s.fx = current_x + (r1 / std::default_random_engine::max());
		s.fy = current_y + (r2 / std::default_random_engine::max());
		
		++current_sample;
		//std::cout << "succefull next sample" << std::endl;
		return true;
	}
	
	int SceneRenderer::RenderingJob::VirtualExecute()
	{
		const int max_depth = renderOptions.getReflectionBounce();
		
		// generer l'origine et l'extremite du rayon
		Orbiter camera = Context->SceneRef->getOrbiter();
		Point o = camera.position();
		Point p;
		Point e;
		Vector dx, dy;
		
		Sampler::Sample current_sample;
		
		camera.frame(renderOptions.getWidth(), renderOptions.getHeight(), 0, renderOptions.getFov(), p, dx, dy);
		
		while (sampler->getNextSample(current_sample))
        {
			e = p + current_sample.fx * dx + current_sample.fy * dy;

			Ray ray = make_ray(o, e);
			Hit hit;

			// calculer les intersections
			if (Context->RendererRef->castRay(ray, hit))
			{
				//std::cout << "before x=" << current_sample.x << " y= " << current_sample.y << std::endl;
                Color out_color = Context->RendererRef->shade(ray, hit, max_depth);
				(*output)(current_sample.x, current_sample.y) = (*output)(current_sample.x, current_sample.y) + out_color;
				
				//std::cout << "after" << std::endl;
			}
		}
		
		return 0;
	}

	int SceneRenderer::AveragingJob::VirtualExecute()
	{
		for (unsigned int i = 0; i < RenderingJobs.size(); ++i)
        {
            RenderingJobs[i]->WaitFinish();
        }
		RenderingJobs.resize(0);

		std::cout << "Averaging" << std::endl;
		auto samples_per_pixel = renderOptions.getSamplerPerPixels();
		for (int x = 0; x < (*output).width(); ++x)
		{
			for (int y = 0; y < (*output).height(); ++y)
			{
                Color out_color = (*output)(x, y) / (samples_per_pixel);
                out_color.a = 1.0f;
                (*output)(x, y) = out_color;
			}
		}
		
		if (renderOptions.getOutputFilename() != "")
		{
			std::cout << "writing image to "<< renderOptions.getOutputFilename() << std::endl;
			write_image(*output, renderOptions.getOutputFilename().c_str());
		}

		return 0;
	}
	
	bool SceneRenderer::renderScene(Image* image, std::vector<Threading::TJob_ptr>& OutRenderingJobs, const SceneRenderOption_ptr& ro)
	{
		std::cout << "starting rendering" << std::endl;
		if (Context == nullptr)
		{
			std::cerr << "null context use setRenderingContext before calling renderScene" << std::endl;
			return false;
		}

		if (image == nullptr)
		{
			std::cerr << "null image" << std::endl;
			return false;
		}

		const Scene& scene = *Context->SceneRef;
		const SceneRenderOption* ro_to_use = (ro.get() == nullptr) ? &SceneRenderOption::defaultRenderOptions : ro.get();
        const SceneRenderOption& opt = *ro_to_use;

		
        const uint32_t width = opt.getWidth();
        const uint32_t height = opt.getHeight();
        
		if (image->width() != width || image->height() != height)
		{
			std::cerr << "wrong image dimention" << std::endl;
			return false;
		}

        const int sample_per_pixels = opt.getSamplerPerPixels();
    		
		static constexpr uint32_t imageSubDivision = 4;
		static constexpr uint32_t nbJobs = imageSubDivision * imageSubDivision;
		

		//Sampler* samplers[nbJobs];
        RenderingJob_ptr jobs[nbJobs];
		int startX = 0;
		
		std::vector<RenderingJob_ptr> renderingJobs_vector;
		for (uint32_t i = 0; i < imageSubDivision; ++i)
		{
			int startY = 0;
			int endX = std::min(startX + width / imageSubDivision, width);
			for (uint32_t j = 0; j < imageSubDivision; ++j)
			{
				int endY = std::min(startY + height / imageSubDivision, height);
				Sampler* newSampler = new BasicRandomSampler(startX, endX, startY, endY, sample_per_pixels);
				//samplers[i + j * imageSubDivision] = newSampler;
                RenderingJob_ptr newJob = RenderingJob_ptr(new RenderingJob(Context, newSampler, image, opt));
				ThreadingInterface::AddJob(newJob);
				//OutRenderingJobs.push_back(newJob);
				renderingJobs_vector.push_back(newJob);
                jobs[i + j * imageSubDivision] = newJob;
				startY = endY;
			}
			startX = endX;
		}

		auto averagingJob = AveragingJob_ptr(new AveragingJob(Context, image, opt, renderingJobs_vector));
		OutRenderingJobs.push_back(averagingJob);
		ThreadingInterface::AddJob(averagingJob);

		/*
		auto end = std::chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        std::cout << "render time: " << ms << "ms" << std::endl;
		*/
		return true;
	}

	bool SceneRenderer::castRay(const Ray& r, Hit& h) const
	{
		const BBoxBinTree* objTree = Context->SceneRef->getObjects();
		if (objTree == nullptr)
		{
			return false;
		}
		return objTree->intersect(r, h);
	}
	
	bool SceneRenderer::castRay(const Ray& r) const
	{
		const BBoxBinTree* objTree = Context->SceneRef->getObjects();
		if (objTree == nullptr)
		{
			return false;
		}
		Hit h = Hit();
		return objTree->intersect(r, h);
	}

	std::default_random_engine generator;
	//generator.seed(0);
	std::uniform_real_distribution<float> distribution(0, 1);
		
	Color SceneRenderer::shade(const Ray& ray, const Hit& hit, int allowed_child_ray_depth) const
	{
		if (allowed_child_ray_depth <= 0)
		{
			return Color();
		}
		
		const SampleParameters sampleParameters = SampleParameters(hit.uv);
		const Material_ptr material = hit.p_object->getMaterial();
		
		const Color hit_albedo = material->getColor(sampleParameters);
		const float hit_shininess = material->getShininess(sampleParameters);
        const float hit_reflectivity = std::min(material->getReflectivity(sampleParameters), 1.0f);
        const float hit_refraction = material->getRefraction(sampleParameters);
        const float hit_transparency = 1 - hit_albedo.a; // 1 - opacity
        
        static const float delta = 0.001f;
        static const float fresnel_factor_0 = 0.8f;
        
        Color reflectColor = Black();
        Color reflectAndRefractColor = Black();
        //bool isReflective = false;
        //bool isTransparent = false;
        
		bool bReceivedShadow = hit.p_object->getCastShadow();

        // reflected component
        if (hit_transparency > 0 || hit_reflectivity > 0)
        {
			//isReflective = true;
			
			Vector reflectionVector = Tools::reflect(hit.n, ray.direction);
            Ray reflectRay;
            Point p_reflect = hit.p + delta * reflectionVector;
            reflectRay = make_ray(p_reflect, reflectionVector, ray.n);

            Hit reflectHit;

            //lancer du rayon
            if (castRay(reflectRay, reflectHit)) {
                reflectColor = shade(reflectRay, reflectHit, allowed_child_ray_depth - 1);
            }
		}
		
		// refracted component
		if (hit_transparency > 0)
		{
			//isTransparent = true;
			
			Color refractColor = Black();
			const float f = Tools::fresnelFactor(fresnel_factor_0, hit.n, ray.direction);
			float n2 = (ray.emmiter == hit.p_object) ? 1.0f : hit_refraction;

            Vector refractionVector = Tools::refract(hit.n, ray.direction, ray.n, n2);

            Point p = hit.p + delta * refractionVector;
            Ray refractRay = Ray(p, refractionVector, n2, ERayType::Default, hit.p_object);

            Hit refractHit;

			//??
            if (refractionVector.x == 0 && refractionVector.y == 0 && refractionVector.y == 0) {
                refractColor = hit_albedo;
            }
            else if (castRay(refractRay, refractHit)) {
                refractColor = shade(refractRay, refractHit, allowed_child_ray_depth - 1);
            }
            reflectAndRefractColor = f * refractColor + (1 - f) * reflectColor;
		}
        
        Color directIlumination = Black();
        Color lightAmt = Black(), specularColor = Black(); 
        // albedo component
        if (hit_transparency < 1.0f)
        {
			auto lights = Context->SceneRef->getLights();
			lightAmt = Black();
			specularColor = Black(); 
			Point shadowPointOrig = (dot(ray.direction, hit.n) < 0) ? 
				hit.p + hit.n * delta : 
				hit.p - hit.n * delta; 

			/*for (auto light : lights) { 
				//std::cout << "a light\n";
				Vector lightDir = light->getPosition() - hit.p; 
				// square of the distance between hitPoint and the light
				float lightDistance2 = dot(lightDir, lightDir);
				if (light->getRange() * light->getRange() <= lightDistance2)
				{
					//std::cout << "out of range\n";
					continue;
				}
				lightDir = normalize(lightDir); 
				float LdotN = std::max(0.f, dot(lightDir, hit.n)); 
				Ray shadowRay = Ray(shadowPointOrig, lightDir);
				Hit shadowHit;
				bool bShadowHit = castRay(scene, shadowRay, shadowHit);
				float inShadow = (bShadowHit && shadowHit.t * shadowHit.t < lightDistance2) ? 1.0f : 0.0f;
				Color light_color = light->attenuation(hit.p);
				//std::cout << "(1 - inShadow) * light_color * LdotN" << (1 - inShadow) * light_color.r * LdotN << '\n';
				lightAmt = lightAmt + (1 - inShadow) * light_color * LdotN; 
				Vector reflectionDirection = Tools::reflect(-lightDir, hit.n); 
				specularColor = specularColor + std::pow(std::max(0.f, -dot(reflectionDirection, ray.direction)), hit_shininess) * light_color;
			} */
			for (auto light : lights) { 
				//std::cout << "a light\n";
				Vector lightDir = light->getLightDirection(hit.p); 
				
				if (!light->isLightInRange(hit.p))
				{
					//std::cout << "out of range\n";
					continue;
				}
				lightDir = normalize(lightDir); 
				float LdotN = std::max(0.f, dot(lightDir, hit.n)); 
				Color light_color;
				float inShadow = light->ShadePoint(Context, shadowPointOrig, light_color);
				
				if (!bReceivedShadow)
				{
					inShadow = 0.0f;
				}
				//std::cout << "inShadow: " << inShadow << " LdotN: " << LdotN << '\n';
				lightAmt = lightAmt + (1 - inShadow) * light_color * LdotN; 
				Vector reflectionDirection = Tools::reflect(-lightDir, hit.n); 
				specularColor = specularColor + std::pow(std::max(0.f, -dot(reflectionDirection, ray.direction)), hit_shininess) * light_color;
			}
			//albedoColor = lightAmt * hit_albedo * 0.8f/*kd*/ + specularColor * 0.2f /*Ks*/; 
			directIlumination = lightAmt * 0.8f/*kd*/ + specularColor * 0.2f /*Ks*/; 
		}
		
		//albedoColor = (hit_reflectivity > 0.0f) ? (hit_reflectivity * reflectColor + (1 - hit_reflectivity) * albedoColor) : albedoColor;
		//Color directIlumination = albedoColor * hit_albedo.a + hit_transparency * reflectAndRefractColor;
		directIlumination.a = 1.0f;
		
		//return directIlumination * hit_albedo;
		Color indirectIlumination = Black();
		static const uint32_t nbSample = 32;
		
		Vector Nt, Nb;
		createCoordinateSystem(hit.n, Nt, Nb);
		static constexpr float pdf = 1 / (2 * M_PI);
		for (uint32_t n = 0; n < nbSample; ++n)
		{
			float r1 = distribution(generator);
			float r2 = distribution(generator);
			Vector sample = uniformSampleHemisphere(r1, r2);
			Vector sampleWorld = rotateToCoordinateSystem(sample, hit.n, Nt, Nb);
			Ray undirectRay = Ray(hit.p + hit.n * delta, sampleWorld, ray.n);
			Hit undirectHit;
			if (castRay(undirectRay, undirectHit))
			{
				indirectIlumination = indirectIlumination + r1 * shade(undirectRay, undirectHit, allowed_child_ray_depth - 1) / pdf;
			}
		}
		indirectIlumination = indirectIlumination / (float)(nbSample);
		
		//Color hitColor = ((directIlumination + indirectIlumination) * hit_albedo / M_PI);
		Color hitColor = ((directIlumination + indirectIlumination) * hit_albedo / M_PI) * ( 1.0f - hit_reflectivity) + hit_reflectivity * reflectColor; 
		//Color directPlusIndirect = (directIlumination / M_PI + 2 * indirectIlumination) * hit_albedo;
		//ca fausse probablement pas mal de chose
		//directPlusIndirect = (hit_reflectivity > 0.0f) ? (hit_reflectivity * reflectColor + (1 - hit_reflectivity) * directPlusIndirect) : directPlusIndirect;
		return hitColor;
	}

	void SceneRenderer::createCoordinateSystem(const Vector& N, Vector& Nt, Vector& Nb)
	{
		if (std::fabs(N.x) > std::fabs(N.y))
			Nt = Vector(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
		else
			Nt = Vector(0, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
		Nb = cross(N, Nt);
	}
	
	Vector SceneRenderer::uniformSampleHemisphere(const float &r1, const float &r2)
	{
		// cos(theta) = r1 = y
		// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
		float sinTheta = sqrtf(1 - r1 * r1);
		float phi = 2 * M_PI * r2;
		float x = sinTheta * cosf(phi);
		float z = sinTheta * sinf(phi);
		return Vector(x, r1, z);
	} 
	
	Vector SceneRenderer::rotateToCoordinateSystem(const Vector& v, const Vector& N, const Vector& Nt, const Vector& Nb)
	{
		return Vector(
			v.x * Nb.x + v.y * N.x + v.z * Nt.x,
			v.x * Nb.y + v.y * N.y + v.z * Nt.y,
			v.x * Nb.z + v.y * N.z + v.z * Nt.z
		); 
	}

} // end namespace FalconEye
