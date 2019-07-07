#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <cstdint>
#include <map>
#include <memory>

#include "LuaInterface/luaObjectInterface.h"
#include "ray.h"
#include "Utils/resourceManager.h"
#include "vec.h"

using LuaIntf::_opt;
using LuaIntf::_def;

namespace FalconEye {
	
	namespace TextureChannel {
		enum class Type : std::int8_t {

			None 	= 	0b0000,
			Red 	= 	0b0001,
			Green 	= 	0b0010,
			Blue 	= 	0b0100,
			Alpha 	= 	0b1000,
			RGB 	= 	Red | Green | Blue,
			RGBA 	= 	Red | Green | Blue | Alpha
		};
		
		LUA_ENUM_BEGIN_WRITE_FUNTION(TextureChannel)
			LUA_ENUM_WRITE_VALUE(TextureChannel, None)
			LUA_ENUM_WRITE_VALUE(TextureChannel, Red)
			LUA_ENUM_WRITE_VALUE(TextureChannel, Green)
			LUA_ENUM_WRITE_VALUE(TextureChannel, Blue)
			LUA_ENUM_WRITE_VALUE(TextureChannel, RGB)
			LUA_ENUM_WRITE_VALUE(TextureChannel, RGBA)
			LUA_ENUM_END_WRITE_FUNCTION
	} // texture channel end
    
	namespace TextureType {
		enum class Type : std::int16_t {

			None 		= 	0b0000'0000'0000'0000,
			Albedo 		= 	0b0000'0000'0000'0001,
			SubAlbedo 	= 	0b0000'0000'0000'0010,
			Normal 		= 	0b0000'0000'0000'0100,
			SubNormal 	= 	0b0000'0000'0000'1000,
			Smoothness 	= 	0b0000'0000'0001'0000,
			Metalness 	= 	0b0000'0000'0010'0000,
			Specular 	= 	0b0000'0000'0100'0000,
			Occlusion 	= 	0b0000'0000'1000'0000,
			Emissive 	= 	0b0000'0001'0000'0000
		};
		
		LUA_ENUM_BEGIN_WRITE_FUNTION(TextureType)
			LUA_ENUM_WRITE_VALUE(TextureType, None)
			LUA_ENUM_WRITE_VALUE(TextureType, Albedo)
			LUA_ENUM_WRITE_VALUE(TextureType, SubAlbedo)
			LUA_ENUM_WRITE_VALUE(TextureType, Normal)
			LUA_ENUM_WRITE_VALUE(TextureType, SubNormal)
			LUA_ENUM_WRITE_VALUE(TextureType, Smoothness)
			LUA_ENUM_WRITE_VALUE(TextureType, Metalness)
			LUA_ENUM_WRITE_VALUE(TextureType, Specular)
			LUA_ENUM_WRITE_VALUE(TextureType, Occlusion)
			LUA_ENUM_WRITE_VALUE(TextureType, Emissive)
			LUA_ENUM_END_WRITE_FUNCTION
		
	} // texture type end
    

    struct TextureInfo {

        Image_ptr texture;
        TextureType::Type textureType;
        TextureChannel::Type channel;

        //TextureInfo() = delete;
        TextureInfo(Image_ptr img = nullptr, TextureType::Type t = TextureType::Type::None, TextureChannel::Type c = TextureChannel::Type::None)
            : texture(img)
            , textureType(t)
            , channel(c)
        {}

        friend bool operator<(const TextureInfo &t1, const TextureInfo &t2) { return t1.textureType < t2.textureType; }
    };
    
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    class Material;
    typedef std::shared_ptr<Material> Material_ptr;
    

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------
    
    struct SampleParameters {
		vec2 uv;
		
		SampleParameters(const vec2& vec) : uv(vec) {}
	};

	class ColorSampler {
	public:
		ColorSampler() = default;
		ColorSampler(const ColorSampler&) = default;
	public:
		virtual Color sample(const SampleParameters &p) const { return Color(); }//= 0;
		
		LUA_BEGIN_BIND_METHODS(ColorSampler)
            LUA_BIND_CONSTRUCTOR_SP_NOARGS(ColorSampler)
            LUA_END_BIND_METHODS
	};
	
	using ColorSampler_ptr = std::shared_ptr<ColorSampler>;
	
	
	class ScalarSampler {
	public:
		ScalarSampler() = default;
		ScalarSampler(const ScalarSampler&) = default;
	public:
		virtual float sample(const SampleParameters &p) const { return float(); }//= 0;
		
		LUA_BEGIN_BIND_METHODS(ScalarSampler)
            LUA_BIND_CONSTRUCTOR_SP_NOARGS(ScalarSampler)
            LUA_END_BIND_METHODS
	};
	
	using ScalarSampler_ptr = std::shared_ptr<ScalarSampler>;
	
	class NormalSampler {
	public:
		NormalSampler() = default;
		NormalSampler(const NormalSampler&) = default;
	public:
		virtual Vector sample(const SampleParameters &p) const { return Vector(0.0f, 0.0f, 1.0f); }//= 0;
		
		LUA_BEGIN_BIND_METHODS(NormalSampler)
            LUA_BIND_CONSTRUCTOR_SP_NOARGS(NormalSampler)
            LUA_END_BIND_METHODS
	};
	
	using NormalSampler_ptr = std::shared_ptr<NormalSampler>;
	
	// ----------------------------------------------------------------------
	
	class ConstantColorSampler : public ColorSampler {
	protected:
		Color value;
	public:
		ConstantColorSampler() = delete;
		ConstantColorSampler(const Color& c) : ColorSampler(), value(c) {} 
		virtual Color sample(const SampleParameters &p) const override {
			return value;
		}
		
		LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(ConstantColorSampler, ColorSampler)
            LUA_BIND_CONSTRUCTOR_SP(ConstantColorSampler, Color)
            LUA_END_BIND_METHODS
	};
	
	// ----------------------------------------------------------------------
	
	class TextureColorSampler : public ColorSampler {
	protected:
		Image_ptr texture;
	public:
		TextureColorSampler() = delete;
		TextureColorSampler(const std::string& image_path);
		
		virtual Color sample(const SampleParameters &p) const override;
		
		LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(TextureColorSampler, ColorSampler)
            LUA_BIND_CONSTRUCTOR_SP(TextureColorSampler, std::string)
            LUA_END_BIND_METHODS
	};
	
	// ----------------------------------------------------------------------
	
	class ConstantScalarSampler : public ScalarSampler {
	protected:
		float value;
	public:
		ConstantScalarSampler() = delete;
		ConstantScalarSampler(float v) : ScalarSampler(), value(v) {}
		virtual float sample(const SampleParameters &p) const override {
			return value;
		}
		
		LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(ConstantScalarSampler, ScalarSampler)
            LUA_BIND_CONSTRUCTOR_SP(ConstantScalarSampler, float)
            LUA_END_BIND_METHODS
	};
	
	// ----------------------------------------------------------------------
	
	class TextureScalarSampler : public ScalarSampler {
	protected:
		Image_ptr texture;
		TextureChannel::Type channel;
	public:
		TextureScalarSampler() = delete;
		TextureScalarSampler(const std::string& image_path, int);
		virtual float sample(const SampleParameters &p) const override;
		
		LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(TextureScalarSampler, ScalarSampler)
            LUA_BIND_CONSTRUCTOR_SP(TextureScalarSampler, std::string, int)
            LUA_END_BIND_METHODS
	};
	
	// ----------------------------------------------------------------------
	
	class TextureNormalSampler : public NormalSampler {
	protected:
		Image_ptr texture;
	public:
		TextureNormalSampler() = delete;
		TextureNormalSampler(const std::string& image_path);
		virtual Vector sample(const SampleParameters &p) const override;
		
		LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(TextureNormalSampler, NormalSampler)
            LUA_BIND_CONSTRUCTOR_SP(TextureNormalSampler, std::string)
            LUA_END_BIND_METHODS
	};
	
    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    class Material {
    private:
		
		ColorSampler_ptr albedoSampler;
		NormalSampler_ptr normalSampler;
		ScalarSampler_ptr shininessSampler;
		ScalarSampler_ptr reflectivitySampler;
		ScalarSampler_ptr refractionSampler;
		
    public:
        Material(ColorSampler_ptr c, NormalSampler_ptr n, ScalarSampler_ptr s, ScalarSampler_ptr r, ScalarSampler_ptr i);

        Material(const Material&) = default;

		bool hasAlbedoSampler() const { return albedoSampler.get() != nullptr; }
		bool hasNormalSampler() const { return normalSampler.get() != nullptr; }
		bool hasShininessSampler() const { return shininessSampler.get() != nullptr; }
		bool hasReflectivitySampler() const { return reflectivitySampler.get() != nullptr; }
		bool hasRefractionSampler() const { return refractionSampler.get() != nullptr; }
		
        Color getColor(const SampleParameters& p) const;
        float getShininess(const SampleParameters& p) const;
        float getReflectivity(const SampleParameters& p) const;
        float getRefraction(const SampleParameters& p) const;
        
        LUA_BEGIN_BIND_METHODS(Material)
            LUA_BIND_CONSTRUCTOR_SP(Material, ColorSampler_ptr, NormalSampler_ptr, ScalarSampler_ptr, ScalarSampler_ptr, ScalarSampler_ptr)
            LUA_END_BIND_METHODS

    };

    typedef std::shared_ptr<Material> Material_ptr;
} // end namespace FalconEye

#endif
