#ifndef RAY_HPP
#define RAY_HPP

#include "gKit/color.h"
#include "gKit/vec.h"

namespace FalconEye {
    class SceneObject;

    struct Ray
    {
        Point origin;
        Vector direction;
        //refraction of the environment
        float n;
    };

    Ray make_ray(const Point& o, const Point& e, float i = 1);
    Ray make_ray(const Point& o, const Vector& d, float i = 1);

    class SceneObject;

    struct Hit
    {
        Point p;
        Vector n;
        float t;
        SceneObject *p_object;
        vec2 uv;
        Color c;

        Hit();
    };
} // end namespace FalconEye

#endif
