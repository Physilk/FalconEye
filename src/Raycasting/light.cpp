#include <algorithm>
#include <math.h>
#include <cstdlib>

#include "light.h"
#include "sceneRenderer.h"
#include "ray.h"

namespace FalconEye {
    Color Light::attenuation(const Point & p) const
    {
        Vector L = position - p;
        float distance = length(L);
        //distance = std::max(0.0f, distance - range);
        Color ret = color * (1.0f / (attenuationParameters.constant
            + attenuationParameters.linear * distance
            + (attenuationParameters.quadratic * distance * distance)));
        ret.a = 1.0f;
        //std::cerr << "cst = " << attenuationParameters.constant << " linear= " << attenuationParameters.linear << " quadratic = " << attenuationParameters.quadratic << '\n';
        return ret;
    }
    
    float PointLight::ShadePoint(const Scene& scene, const Point& point, Color& outColor) const
    {		
		Vector lightDir = this->getPosition() - point; 
		// square of the distance between hitPoint and the light
		float lightDistance2 = length2(lightDir);
		lightDir = normalize(lightDir); 
		Ray shadowRay = Ray(point, lightDir);
		Hit shadowHit;
		bool bShadowHit = SceneRenderer::castRay(scene, shadowRay, shadowHit);
		float inShadow = (bShadowHit && shadowHit.t * shadowHit.t < lightDistance2) ? 1.0f : 0.0f;
		//std::cout << "shadowHit.t: " << shadowHit.t * shadowHit.t << " dist: " <<  lightDistance2 << "\n";
		outColor = this->attenuation(point);
		return inShadow;
	}
	
	float irand(std::default_random_engine& random_engine, int min, int max) 
	{
		return (random_engine() / (std::default_random_engine::max() + 1.0)) * (max - min) + min;
	}

	float SphereLight::ShadePoint(const Scene& scene, const Point& point, Color& outColor) const
    {		
		float inShadowSum = 0.0f;
		
		for (unsigned int i = 0; i < NbSamples; ++i)
		{
			Vector lightDir = getRandomPointOnSphere(point) - point;
			float lightDistance2 = length2(lightDir);
			lightDir = normalize(lightDir);
			Ray shadowRay = Ray(point, lightDir);
			Hit shadowHit;
			bool bShadowHit = SceneRenderer::castRay(scene, shadowRay, shadowHit);
			inShadowSum += (bShadowHit && shadowHit.t * shadowHit.t < lightDistance2) ? 1.0f : 0.0f;
		}
		
		outColor = this->attenuation(point);
		return inShadowSum / NbSamples;
		
	}
	
	Point SphereLight::getRandomPointOnSphere(const Point& shadowPoint) const
	{
		static std::default_random_engine random_engine;
		
		Vector p;
		float theta = 2*M_PI*irand(random_engine, 0, 1);
		// corrrect
		float phi = acos(2*irand(random_engine, 0, 1)-1.0);
		// incorrect
		//phi = PI*irand(0,1);
		p.x = cos(theta)*sin(phi);
		p.y = sin(theta)*sin(phi);
		p.z = cos(phi);
		
		Vector dir = Point(p) - shadowPoint;
		dir = normalize(dir);
		if (dot(p, dir) < 0)
		{
			p = p * -1.0f;
		}
		return this->getPosition() + p * getRadius();
	}
} // end namespace FalconEye
