#include <math.h>

#include "material.h"

using std::cout;

namespace FalconEye {
    Material::Material(Color c, float s, float r, float i) : color(nullptr),
        width(1),
        height(1),
        shininess(s),
        reflectivity(r),
        n(i),
        texture(false),
        defaultValue(c, s, r, i)
    {
        reserve();
        color[0] = c;
    }

    Material::Material(const std::string &file_name, float s, float r, float i) : color(nullptr),
        width(1),
        height(1),
        shininess(s),
        reflectivity(r),
        n(i),
        texture(false),
        image(ResourceManager::Instance().requestImage(file_name)),
        defaultValue(Color(0.85f, 0.85f, 0.85f), s, r, i)
    {
        texture = true;
        color = (Color*)(image->buffer());
        width = image->width();
        height = image->height();
    }


    void Material::reserve() {
        color = new Color[width * height];
    }

    void Material::free() {
        delete color;
    }

    Color Material::getColor(float u, float v) const {
        size_t u_coord, v_coord; //coord en pixel

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

        return color[u_coord + v_coord * width];
    }

    void Material::addTexture(TextureInfo ti)
    {
        if (useTexture(ti.textureType))
        {
            auto it = textures.find(ti.textureType);
            textures.erase(ti.textureType);
        }

        usedTextureMask |= static_cast<std::int16_t>(ti.textureType);
        textures[ti.textureType] = ti;
    }

    void Material::addAlbedoTexture(const std::string & file_name)
    {
        Image_ptr img = ResourceManager::Instance().requestImage(file_name);
        if (img != nullptr)
        {
            addTexture(TextureInfo(img, TextureType::Albedo, TextureChannel::RGB));
        }
    }

    const TextureInfo Material::getTexture(TextureType t)
    {
        if (textures.find(t) != textures.end())
        {
            return textures[t];
        }
        return TextureInfo();
    }

    Color TextureSamplerAlbedoMaterialModule::process(const Material_ptr mat, const Hit & h)
    {
        float u = h.uv.x * u_multiplier;
        float v = h.uv.y * v_multiplier;
        const TextureInfo& ti = mat->getAlbedoTexture();

        if (ti.texture != nullptr)
        {
            size_t u_coord, v_coord; //coord en pixel
            const Image_ptr& img = ti.texture;
            const size_t& width = img->width();
            const size_t& height = img->height();

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

            return (*img)(u_coord, v_coord);
        }

        return Color();
    }

    Color ConstantAlbedoMaterialModule::process(const Material_ptr m, const Hit &h)
    {
        if (h.c.r != -1.0f)
        {
            return h.c;
        }
        return Color(0.0f, 0.0f, 0.0f);
    }
} // end namespace FalconEye
