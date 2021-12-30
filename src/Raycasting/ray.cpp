#include "ray.h"

namespace FalconEye {
	
	Ray::Ray(const Point& o, const Point& e, float i, ERayType t, const SceneObject* obj)
		: origin(o)
		, direction()
		, n(i)
		, emmiter(obj)
        , rayType(t)
		{
			direction = normalize(Vector(o, e));
		}
        
	Ray::Ray(const Point& o, const Vector& v, float i, ERayType t, const SceneObject* obj)
		: origin(o)
		, direction(v)
		, n(i)
		, emmiter(obj)
        , rayType(t)
		{}
			
    Ray make_ray(const Point& o, const Point& e, float i)
    {
        Ray r;
        r.origin = o;
        r.direction = normalize(Vector(o, e));
        r.n = i;
        return r;
    }


    Ray make_ray(const Point& o, const Vector& d, float i) {

        Ray r;
        r.origin = o;
        r.direction = d;
        r.n = i;
        return r;
    }

    Hit::Hit() : p(Point(0, 0, 0)),
        n(Vector(0, 0, 0)),
        t(0),
        p_object(nullptr),
        uv(vec2(0, 0)),
        c(-1.0f, -1.0f, -1.0f)
    {}

} // end namespace FalconEye
