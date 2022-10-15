#ifndef RAY_HPP
#define RAY_HPP

#include "gKit/color.h"
#include "gKit/vec.h"

namespace FalconEye {
    class SceneObject;

    enum ERayType : uint8_t
    {
        Default,
        Shadow
    };

    struct Ray
    {
        Point origin;
        Vector direction;
        //refraction of the environment
        float n;
        const SceneObject* emmiter;
        
        ERayType rayType = ERayType::Default;
        float maxDistance = -1.0f;

        Ray() = default;
        Ray(const Point& o, const Point& e, float i = 1, ERayType t = ERayType::Default, const SceneObject* obj = nullptr);
        Ray(const Point& o, const Vector& v, float i = 1, ERayType t = ERayType::Default, const SceneObject* obj = nullptr);
    };

    Ray make_ray(const Point& o, const Point& e, float i = 1); 
    Ray make_ray(const Point& o, const Vector& d, float i = 1);

    struct Hit
    {
        Point p;
        Vector n;
        float t;
        const SceneObject *p_object;
        vec2 uv;
        Color c;

        Hit();
    };
} // end namespace FalconEye

#endif
