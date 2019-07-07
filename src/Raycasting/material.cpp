#include <math.h>

#include "material.h"

using std::cout;

namespace FalconEye {
    
    Material::Material(ColorSampler_ptr c, NormalSampler_ptr n, ScalarSampler_ptr s, ScalarSampler_ptr r, ScalarSampler_ptr i)
		: albedoSampler(c)
		, normalSampler(n)
		, shininessSampler(s)
		, reflectivitySampler(r)
		, refractionSampler(i)
    {
    }

    Color Material::getColor(const SampleParameters& p) const {
        if (albedoSampler != nullptr)
        {
			return albedoSampler->sample(p);
		}
		return Color();
    }
    
    float Material::getShininess(const SampleParameters& p) const {
		if (shininessSampler != nullptr)
		{
			return shininessSampler->sample(p);
		}
		return float();
	}
    
    float Material::getReflectivity(const SampleParameters& p) const {
		if (reflectivitySampler != nullptr)
		{
			return reflectivitySampler->sample(p);
		}
		return float();
	}
	
    float Material::getRefraction(const SampleParameters& p) const {
		if (refractionSampler != nullptr)
		{
			return refractionSampler->sample(p);
		}
		return float();
	}
	
	//------------------------------------------------------------------
	//------------------------------------------------------------------
    
    TextureColorSampler::TextureColorSampler(const std::string& image_path)
		: ColorSampler()
		, texture(nullptr)
    {
		texture = ResourceManager::Instance().requestImage(image_path);
	}
		
	Color TextureColorSampler::sample(const SampleParameters& p) const
	{
		if (texture != nullptr)
		{	
			float u = p.uv.x;
			float v = p.uv.y;		
			size_t u_coord, v_coord; //coord en pixel
            const size_t& width = texture->width();
            const size_t& height = texture->height();

            //on s'assure que u et v sont entre 0 et 1
            if (u < 0)
                u = 1 + (u + std::ceil(u));
            else if (u > 1)
                u = u - std::floor(u);
            if (v < 0)
                v = 1 + (v + std::ceil(v));
            else if (v > 1)
                v = v - std::floor(v);

            u_coord = u * width - 1;
            if (u_coord > width)
                u_coord = 0;
            v_coord = v * height - 1;
            if (v_coord > height)
                v_coord = 0;

            return (*texture)(u_coord, v_coord);
		} else {
			return Color(0.25f, 0.25f, 0.25f);
		}
	}
	
	//------------------------------------------------------------------
	
	TextureScalarSampler::TextureScalarSampler(const std::string& image_path, int chan)
		: ScalarSampler()
		, texture(nullptr)
		, channel(TextureChannel::Type::None)
    {
		texture = ResourceManager::Instance().requestImage(image_path);
		channel = static_cast<TextureChannel::Type>(chan);
	}
		
	float TextureScalarSampler::sample(const SampleParameters& p) const
	{
		Color c = Color();
		if (texture != nullptr)
		{	
			float u = p.uv.x;
			float v = p.uv.y;		
			size_t u_coord, v_coord; //coord en pixel
            const size_t& width = texture->width();
            const size_t& height = texture->height();

            //on s'assure que u et v sont entre 0 et 1
            if (u < 0)
                u = 1 + (u + std::ceil(u));
            else if (u > 1)
                u = u - std::floor(u);
            if (v < 0)
                v = 1 + (v + std::ceil(v));
            else if (v > 1)
                v = v - std::floor(v);

            u_coord = u * width - 1;
            if (u_coord > width)
                u_coord = 0;
            v_coord = v * height - 1;
            if (v_coord > height)
                v_coord = 0;

            c = (*texture)(u_coord, v_coord);
		}
		const int&& chan_int = static_cast<int>(channel); 
		if (chan_int & static_cast<int>(TextureChannel::Type::Red))
		{
			return c.r;
		}
		if (chan_int & static_cast<int>(TextureChannel::Type::Green))
		{
			return c.g;
		}
		if (chan_int & static_cast<int>(TextureChannel::Type::Blue))
		{
			return c.b;
		}
		
		return c.a;
	}
	
	//------------------------------------------------------------------
	
	TextureNormalSampler::TextureNormalSampler(const std::string& image_path)
		: NormalSampler()
		, texture(nullptr)
    {
		texture = ResourceManager::Instance().requestImage(image_path);
	}
		
	Vector TextureNormalSampler::sample(const SampleParameters& p) const
	{
		if (texture != nullptr)
		{	
			float u = p.uv.x;
			float v = p.uv.y;		
			size_t u_coord, v_coord; //coord en pixel
            const size_t& width = texture->width();
            const size_t& height = texture->height();

            //on s'assure que u et v sont entre 0 et 1
            if (u < 0)
                u = 1 + (u + std::ceil(u));
            else if (u > 1)
                u = u - std::floor(u);
            if (v < 0)
                v = 1 + (v + std::ceil(v));
            else if (v > 1)
                v = v - std::floor(v);

            u_coord = u * width - 1;
            if (u_coord > width)
                u_coord = 0;
            v_coord = v * height - 1;
            if (v_coord > height)
                v_coord = 0;

			const Color& c = (*texture)(u_coord, v_coord);
            return Vector(c.r, c.g, c.b);
		} else {
			return Vector(0.0f, 0.0f, 1.0f);
		}
	}
} // end namespace FalconEye
