#include "sceneRenderer.h"
#include "scene.h"
#include "ray.h"
#include "bintree.h"
#include "tools.h"
#include "material.h"

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
		
		s.fx = current_x + (random_engine() / std::default_random_engine::max());
		s.fy = current_y + (random_engine() / std::default_random_engine::max());
		
		++current_sample;
		return true;
	}
	
	Image renderScene(const Scene& s, const SceneRenderOption_ptr& ro)
	{
		std::cout << "starting rendering" << std::endl;
		const SceneRenderOption* ro_to_use = (ro.get() == nullptr) ? &SceneRenderOption::defaultRenderOptions : ro.get();
        const SceneRenderOption& opt = *ro_to_use;

        const int width = opt.getWidth();
        const int height = opt.getHeight();
        const int max_depth = opt.getReflectionBounce();
        
        static const int sample_per_pixels = 1;
        Sampler::Sample current_sample;
        
        Image output_image(width, height);
        BasicRandomSampler sampler(0, width, 0, height, sample_per_pixels);
        
		// generer l'origine et l'extremite du rayon
		Orbiter camera = s.getOrbiter();
		Point o = camera.position();
		Point p;
		Point e;
		Vector dx, dy;
		
		camera.frame(width, height, 0, opt.getFov(), p, dx, dy);

        while (sampler.getNextSample(current_sample))
        {
			e = p + current_sample.fx * dx + current_sample.fy * dy;

			Ray ray = make_ray(o, e);
			Hit hit;

			// calculer les intersections
			if (castRay(s, ray, hit)) {
				//std::cout << "before x=" << current_sample.x << " y= " << current_sample.y << std::endl;
				output_image(current_sample.x, current_sample.y) = output_image(current_sample.x, current_sample.y) + shade(s, ray, hit, max_depth);
				
				//std::cout << "after" << std::endl;
			}
		}
		
		std::cout << "Averraging" << std::endl;
		for (int x = 0; x < width; ++x)
		{
			for (int y = 0; y < height; ++y)
			{
				output_image(x, y) = output_image(x, y) / sample_per_pixels;
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
        
        Color albedoColor = Black();
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
			albedoColor = lightAmt * hit_albedo * 0.8f/*kd*/ + specularColor * 0.2f /*Ks*/; 
		}
		//std::cout << "albedo rgba=" << albedoColor.r << ' ' << albedoColor.g << ' ' << albedoColor.b << ' ' << albedoColor.a << '\n';
		//std::cout << "hit_albedo rgba=" << hit_albedo.r << ' ' << hit_albedo.g << ' ' << hit_albedo.b << ' ' << hit_albedo.a << '\n';
		//std::cout << "specularColor rgba=" << specularColor.r << ' ' << specularColor.g << ' ' << specularColor.b << ' ' << specularColor.a << '\n';
		//std::cout << "lightAmt rgba=" << lightAmt.r << ' ' << lightAmt.g << ' ' << lightAmt.b << ' ' << lightAmt.a << '\n';
		albedoColor = (hit_reflectivity > 0.0f) ? (hit_reflectivity * reflectColor + (1 - hit_reflectivity) * albedoColor) : albedoColor;
		//std::cout << "albedo rgba=" << albedoColor.r << ' ' << albedoColor.g << ' ' << albedoColor.b << ' ' << albedoColor.a << "reflectivity=" << hit_reflectivity<<'\n';
		Color directIlumination = albedoColor * hit_albedo.a + hit_transparency * reflectAndRefractColor;
		
		directIlumination.a = 1.0f;
		return directIlumination;
	}

    
	} // end namespace SeneRenderer

} // end namespace FalconEye
