#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <cstdint>
#include <map>
#include <memory>

#include "luaObjectInterface.h"
#include "ray.h"
#include "resourceManager.h"

using LuaIntf::_opt;
using LuaIntf::_def;

namespace FalconEye {
    enum class TextureChannel : std::int8_t {

        None = 0b0000,
        Red = 0b0001,
        Green = 0b0010,
        Blue = 0b0100,
        Alpha = 0b1000,
        RGB = Red | Green | Blue,
        RGBA = Red | Green | Blue | Alpha
    };

    enum class TextureType : std::int16_t {

        None = 0b0000'0000'0000'0000,
        Albedo = 0b0000'0000'0000'0001,
        SubAlbedo = 0b0000'0000'0000'0010,
        Normal = 0b0000'0000'0000'0100,
        SubNormal = 0b0000'0000'0000'1000,
        Smoothness = 0b0000'0000'0001'0000,
        Metalness = 0b0000'0000'0010'0000,
        Specular = 0b0000'0000'0100'0000,
        Occlusion = 0b0000'0000'1000'0000,
        Emissive = 0b0000'0001'0000'0000
    };

    struct TextureInfo {

        Image_ptr texture;
        TextureType textureType;
        TextureChannel channel;

        //TextureInfo() = delete;
        TextureInfo(Image_ptr img = nullptr, TextureType t = TextureType::None, TextureChannel c = TextureChannel::None)
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

    class AlbedoMaterialModule {
    protected:
        AlbedoMaterialModule() = default;
        AlbedoMaterialModule(const AlbedoMaterialModule&) = default;

        virtual ~AlbedoMaterialModule() = 0;

    public:
        virtual Color process(const Material_ptr, const Hit&) = 0;
    };

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    class TextureSamplerAlbedoMaterialModule : public AlbedoMaterialModule {
    private:
        float u_multiplier;
        float v_multiplier;
    public:
        TextureSamplerAlbedoMaterialModule(float um = 1.0f, float vm = 1.0f)
            : AlbedoMaterialModule()
            , u_multiplier(um)
            , v_multiplier(vm)
        {}
        TextureSamplerAlbedoMaterialModule(const TextureSamplerAlbedoMaterialModule&) = default;

        virtual ~TextureSamplerAlbedoMaterialModule() {}

    public:
        virtual Color process(const Material_ptr, const Hit&);
    };

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    class ConstantAlbedoMaterialModule : public AlbedoMaterialModule {
    private:
        float u_multiplier;
        float v_multiplier;
    public:
        ConstantAlbedoMaterialModule(float um = 1.0f, float vm = 1.0f)
            : AlbedoMaterialModule()
            , u_multiplier(um)
            , v_multiplier(vm)
        {}
        ConstantAlbedoMaterialModule(const ConstantAlbedoMaterialModule&) = default;

        virtual ~ConstantAlbedoMaterialModule() {}

    public:
        virtual Color process(const Material_ptr, const Hit&);
    };

    // ----------------------------------------------------------------------
    // ----------------------------------------------------------------------

    class Material {
    private:

        //LEGACY
        Color * color;
        size_t width;
        size_t height;

        //coeff de brillance < 1 = pas brilant, 1 = brillance max (objet tout blanc), plus on angmente moins c'est brillant
        float shininess;
        //coeff de reflection, 0 pas reflexif, 1 totalement reflexif
        float reflectivity;

        //indice de refraction
        float n;

        //is the material a texture
        bool texture;

        Image_ptr image;

        void reserve();
        void free();

        //NEW
        std::int16_t usedTextureMask;
        std::map<TextureType, TextureInfo> textures;

        struct DefaultValues
        {
            Color albedo;
            float shininess;
            float reflectivity;
            float refraction;

            DefaultValues(Color c = Color(0.85f, 0.85f, 0.85f), float s = 0, float reflec = 0, float refract = 1)
                : albedo(c)
                , shininess(s)
                , reflectivity(reflec)
                , refraction(refract)
            {}
        } defaultValue;

    public:
        Material(Color c = Color(0.85f, 0.85f, 0.85f), float s = 0, float r = 0, float i = 1);
        Material(const std::string &file_name, float s = 0, float r = 0, float i = 1);

        Material(const Material&) = default;

        ~Material() { if (!texture) { free(); } }

        //Color getColor() const {return color;}
        Color getColor(float u = 1, float v = 1) const;
        float getShininess() const { return shininess; }
        float getReflectivity() const { return reflectivity; }
        float getRefraction() const { return n; }

        bool isTexture() const { return texture; }

        //NEW
        bool useTexture(TextureType t) { return static_cast<std::int16_t>(t) & usedTextureMask; }
        void addTexture(TextureInfo ti);
        void addAlbedoTexture(const std::string &file_name);

        const TextureInfo getTexture(TextureType t);
        const TextureInfo getAlbedoTexture() { return getTexture(TextureType::Albedo); }

        //for LUA
        static std::shared_ptr<Material> make_textured_material(std::string file_name, float s = 0, float r = 0, float i = 1) { return std::shared_ptr<Material>(new Material(file_name, s, r, i)); }

        LUA_BEGIN_BIND_METHODS(Material)
            LUA_BIND_CONSTRUCTOR_SP(Material, _opt<Color>, _opt<float>, _opt<float>, _opt<float>)
            //.addConstructor(LUA_ARGS(_opt<Color>, _opt<float>, _opt<float>, _opt<float>))
            //.addFactory([](Color c, float s, float r, float i) {return std::shared_ptr<Material>(new Material(c, s, r, i)); })
            .addStaticFunction("make_textured_material", &Material::make_textured_material, LUA_ARGS(std::string, _opt<float>, _opt<float>, _opt<float>))
            LUA_END_BIND_METHODS

    };

    typedef std::shared_ptr<Material> Material_ptr;
} // end namespace FalconEye

#endif
