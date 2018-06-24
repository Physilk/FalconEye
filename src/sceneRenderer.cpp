#include "sceneRenderer.h"
#include "scene.h"
#include "ray.h"
#include "bintree.h"
#include "tools.h"
#include "material.h"


using Threading::TThreadManager;

namespace FalconEye {
	namespace SceneRenderer {
		
    SceneRenderOption SceneRenderOption::defaultRenderOptions = SceneRenderOption();
    
    bool BasicRandomSampler::getNextSample(Sample &s)
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
		return true;
	}
	
	int RenderingJob::VirtualExecute()
	{
		const int max_depth = renderOptions->getReflectionBounce();
		
		// generer l'origine et l'extremite du rayon
		Orbiter camera = scene->getOrbiter();
		Point o = camera.position();
		Point p;
		Point e;
		Vector dx, dy;
		
		Sampler::Sample current_sample;
		
		camera.frame(renderOptions->getWidth(), renderOptions->getHeight(), 0, renderOptions->getFov(), p, dx, dy);
		
		while (sampler->getNextSample(current_sample))
        {
			e = p + current_sample.fx * dx + current_sample.fy * dy;

			Ray ray = make_ray(o, e);
			Hit hit;

			// calculer les intersections
			if (castRay(*scene, ray, hit)) {
				//std::cout << "before x=" << current_sample.x << " y= " << current_sample.y << std::endl;
                Color out_color = shade(*scene, ray, hit, max_depth);
				(*output)(current_sample.x, current_sample.y) = (*output)(current_sample.x, current_sample.y) + out_color;
				
				//std::cout << "after" << std::endl;
			}
		}
		
		return 0;
	}
	
	Image renderScene(const Scene& s, const SceneRenderOption_ptr& ro)
	{
		std::cout << "starting rendering" << std::endl;
		const SceneRenderOption* ro_to_use = (ro.get() == nullptr) ? &SceneRenderOption::defaultRenderOptions : ro.get();
        const SceneRenderOption& opt = *ro_to_use;

        const uint32_t width = opt.getWidth();
        const uint32_t height = opt.getHeight();
        
        const int sample_per_pixels = opt.getSamplerPerPixels();
        
        Image output_image(width, height);

		
		static constexpr uint32_t imageSubDivision = 4;
		static constexpr uint32_t nbJobs = imageSubDivision * imageSubDivision;
		

		Sampler* samplers[nbJobs];
        RenderingJob_ptr jobs[nbJobs];
		int startX = 0;
		auto begin = std::chrono::high_resolution_clock::now();
		for (uint32_t i = 0; i < imageSubDivision; ++i)
		{
			int startY = 0;
			int endX = std::min(startX + width / imageSubDivision, width);
			for (uint32_t j = 0; j < imageSubDivision; ++j)
			{
				int endY = std::min(startY + height / imageSubDivision, height);
				Sampler* newSampler = new BasicRandomSampler(startX, endX, startY, endY, sample_per_pixels);
				samplers[i + j * imageSubDivision] = newSampler;
                RenderingJob_ptr newJob = RenderingJob_ptr(new RenderingJob(&s, newSampler, &output_image, ro_to_use));
				ThreadingInterface::AddJob(newJob);
                jobs[i + j * imageSubDivision] = newJob;
				startY = endY;
			}
			startX = endX;
		}
        for (unsigned int i = 0; i < nbJobs; ++i)
        {
            if (jobs[i].get() != nullptr)//shouldn't happen but y'a know...
            {
                jobs[i]->WaitFinish();
            }
        }
		std::cout << "Averaging" << std::endl;
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
                Color out_color = output_image(x, y) / sample_per_pixels;
                //output_image(x, y) = output_image(x, y) / sample_per_pixels;
                /*out_color.r = std::sqrt(out_color.r / sample_per_pixels) / (255.0f);
                out_color.g = std::sqrt(out_color.g / sample_per_pixels) / (255.0f);
                out_color.b = std::sqrt(out_color.b / sample_per_pixels) / (255.0f);*/
                out_color.a = 1.0f;
                output_image(x, y) = out_color;
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        std::cout << "render time: " << ms << "ms" << std::endl;
		
		//cleaning
		for (uint32_t i = 0; i < imageSubDivision; ++i)
		{
			for (uint32_t j = 0; j < imageSubDivision; ++j)
			{
				delete samplers[i + j * imageSubDivision];
			}
		}
		
		return output_image;
	}

	bool castRay(const Scene& s, const Ray& r, Hit& h)
	{
		const BBoxBinTree* objTree = s.getObjects();
		if (objTree == nullptr)
		{
			return false;
		}
		return objTree->intersect(r, h);
	}
	
	bool castRay(const Scene& s, const Ray& r)
	{
		const BBoxBinTree* objTree = s.getObjects();
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
		
	Color shade(const Scene& scene, const Ray& ray, const Hit& hit, int allowed_child_ray_depth)
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
        bool isReflective = false;
        bool isTransparent = false;
        
        // reflected component
        if (hit_transparency > 0 || hit_reflectivity > 0)
        {
			isReflective = true;
			
			Vector reflectionVector = Tools::reflect(hit.n, ray.direction);
            Ray reflectRay;
            Point p_reflect = hit.p + delta * reflectionVector;
            reflectRay = make_ray(p_reflect, reflectionVector, ray.n);

            Hit reflectHit;

            //lancer du rayon
            if (castRay(scene, reflectRay, reflectHit)) {
                reflectColor = shade(scene, reflectRay, reflectHit, allowed_child_ray_depth - 1);
            }
		}
		
		// refracted component
		if (hit_transparency > 0)
		{
			isTransparent = true;
			
			Color refractColor = Black();
			const float f = Tools::fresnelFactor(fresnel_factor_0, hit.n, ray.direction);
			float n2 = (ray.emmiter == hit.p_object) ? 1.0f : hit_refraction;

            Vector refractionVector = Tools::refract(hit.n, ray.direction, ray.n, n2);

            Point p = hit.p + delta * refractionVector;
            Ray refractRay = Ray(p, refractionVector, n2, hit.p_object);

            Hit refractHit;

			//??
            if (refractionVector.x == 0 && refractionVector.y == 0 && refractionVector.y == 0) {
                refractColor = hit_albedo;
            }
            else if (castRay(scene, refractRay, refractHit)) {
                refractColor = shade(scene, refractRay, refractHit, allowed_child_ray_depth - 1);
            }
            reflectAndRefractColor = f * refractColor + (1 - f) * reflectColor;
		}
        
        Color directIlumination = Black();
        Color lightAmt = Black(), specularColor = Black(); 
        // albedo component
        if (hit_transparency < 1.0f)
        {
			const vector<PointLight_ptr>& lights = scene.getPointLights();
			lightAmt = Black();
			specularColor = Black(); 
			Point shadowPointOrig = (dot(ray.direction, hit.n) < 0) ? 
				hit.p + hit.n * delta : 
				hit.p - hit.n * delta; 

			for (auto light : lights) { 
				//std::cout << "a light\n";
				Vector lightDir = light->position - hit.p; 
				// square of the distance between hitPoint and the light
				float lightDistance2 = dot(lightDir, lightDir);
				if (light->range * light->range <= lightDistance2)
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
			if (castRay(scene, undirectRay, undirectHit))
			{
				indirectIlumination = indirectIlumination + r1 * shade(scene, undirectRay, undirectHit, allowed_child_ray_depth - 1) / pdf;
			}
		}
		indirectIlumination = indirectIlumination / (float)(nbSample);
		
		Color hitColor = (directIlumination + indirectIlumination) * hit_albedo / M_PI; 
		//Color directPlusIndirect = (directIlumination / M_PI + 2 * indirectIlumination) * hit_albedo;
		//ca fausse probablement pas mal de chose
		//directPlusIndirect = (hit_reflectivity > 0.0f) ? (hit_reflectivity * reflectColor + (1 - hit_reflectivity) * directPlusIndirect) : directPlusIndirect;
		return hitColor;
	}

	void createCoordinateSystem(const Vector& N, Vector& Nt, Vector& Nb)
	{
		if (std::fabs(N.x) > std::fabs(N.y))
			Nt = Vector(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
		else
			Nt = Vector(0, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
		Nb = cross(N, Nt);
	}
	
	Vector uniformSampleHemisphere(const float &r1, const float &r2)
	{
		// cos(theta) = r1 = y
		// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
		float sinTheta = sqrtf(1 - r1 * r1);
		float phi = 2 * M_PI * r2;
		float x = sinTheta * cosf(phi);
		float z = sinTheta * sinf(phi);
		return Vector(x, r1, z);
	} 
	
	Vector rotateToCoordinateSystem(const Vector& v, const Vector& N, const Vector& Nt, const Vector& Nb)
	{
		return Vector(
			v.x * Nb.x + v.y * N.x + v.z * Nt.x,
			v.x * Nb.y + v.y * N.y + v.z * Nt.y,
			v.x * Nb.z + v.y * N.z + v.z * Nt.z
		); 
	}

    
	} // end namespace SeneRenderer

} // end namespace FalconEye
