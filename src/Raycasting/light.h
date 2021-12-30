#ifndef LIGHT_HPP
#define LIGHT_HPP

//#include <memory>
#include <limits>
#include <random>

#include "gKit/color.h"
#include "gKit/vec.h"

#include "Raycasting/renderingContext.h"
#include "LuaInterface/luaObjectInterface.h"
#include "Raycasting/ray.h"

namespace FalconEye {

	class Scene;
	
	struct AttenuationParameters {
		float constant;
		float linear;
		float quadratic;

		float getConstant() const { return constant; }
		float getLinear() const { return linear; }
		float getQuadratic() const { return quadratic; }

		void setConstant(float c) { constant = c; }
		void setLinear(float l) { linear = l; }
		void setQuadratic(float q) { quadratic = q; }

		AttenuationParameters(float c = 1.0f, float l = 0.0f, float q = 0.0f)
			: constant(c)
			, linear(l)
			, quadratic(q)
		{}

		LUA_BEGIN_BIND_METHODS(AttenuationParameters)
			LUA_BIND_CONSTRUCTOR(float, float, float)
			LUA_BIND_PROPERTY(AttenuationParameters, constant, getConstant, setConstant)
			LUA_BIND_PROPERTY(AttenuationParameters, linear, getLinear, setLinear)
			LUA_BIND_PROPERTY(AttenuationParameters, quadratic, getQuadratic, setQuadratic)
			LUA_END_BIND_METHODS
	};
        
    class Light {
    protected:
		Point position; 
        Color color;
        float range;
        AttenuationParameters attenuationParameters;
        unsigned int NbSamples;
        
    public:
        Light()
        : position()
        , color(1.0f, 1.0f, 1.0f)
        , range(std::numeric_limits<float>::max())
        ,  attenuationParameters()
        , NbSamples(30)
        {}
        
        Light(const Point& p, const Color& c, float r, const AttenuationParameters& a, unsigned int s)
        : position(p)
        , color(c)
        , range(r)
        , attenuationParameters(a)
        , NbSamples(s)
        {}

		virtual ~Light() = default;
		
        Color getColor() const { return color; }
        void setColor(const Color &c) { color = c; }
        
        AttenuationParameters getAttenuation() const { return attenuationParameters; }
        void setAttenuation(const AttenuationParameters &a) { attenuationParameters = a; }
        
        void setRange(float r) { range = r; }
        float getRange() const { return range; }
        
        void setPosition(const Point& p) { position = p; }
        Point getPosition() const { return position; }
        
        void setNbSamples(unsigned int s) { NbSamples = s; }
        unsigned int getNbSamples() const { return NbSamples; }
        
        virtual float ShadePoint(RenderingContext_ptr Context, const Point& point, Color& outColor) const { outColor = Black(); return 1.0f; }
        virtual Color attenuation(const Point &p) const;
        
        virtual Vector getLightDirection(const Point& p) const { return getPosition() - p; }
        virtual bool isLightInRange(const Point& p) const;

        LUA_BEGIN_BIND_METHODS(Light)
            //LUA_BIND_CONSTRUCTOR(Point, float, Color, PointLight::AttenuationParameters)
            .addFactory([](Point p, float r, Color c, AttenuationParameters at) {return std::shared_ptr<Light>(new Light(p, c, r, at, 1u)); })
            LUA_BIND_PROPERTY(Light, position, getPosition, setPosition)
            LUA_BIND_PROPERTY(Light, range, getRange, setRange)
            LUA_BIND_PROPERTY(Light, color, getColor, setColor)
            LUA_END_BIND_METHODS

    protected:
        Ray make_shadow_ray(Point origin, Vector direction, float i = 1.0f) const { return Ray(origin, direction, i, ERayType::Shadow); }
    };

    class PointLight : public Light {
    public:

        PointLight(const Point& p, float r, const Color& c, const AttenuationParameters& ap)
            : Light(p, c, r, ap, 1u)
        {}

        PointLight(const PointLight &) = default;

		virtual ~PointLight() = default;

		float ShadePoint(RenderingContext_ptr Context, const Point& point, Color& outColor) const override;
		//Color attenuation(const Point &p) const override;
		
        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(PointLight, Light)
            //LUA_BIND_CONSTRUCTOR(Point, float, Color, PointLight::AttenuationParameters)
            .addFactory([](Point p, float r, Color c, AttenuationParameters at) {return std::shared_ptr<PointLight>(new PointLight(p, r, c, at)); })
            LUA_END_BIND_METHODS
    };

	class SphereLight : public Light {
	protected:
		float radius;
		
    public:

        SphereLight(const Point& p, float rg, float rad, const Color& c, const AttenuationParameters& ap)
            : Light(p, c, rg, ap, 1u)
            , radius(rad)
        {}

        SphereLight(const SphereLight &) = default;

		virtual ~SphereLight() = default;

		void setRadius(float r) { radius = r; }
        float getRadius() const { return radius; }
        
        Point getRandomPointOnSphere(const Point& shadowPoint) const;
        
		float ShadePoint(RenderingContext_ptr Context, const Point& point, Color& outColor) const override;
		//Color attenuation(const Point &p) const override;
		
        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(SphereLight, Light)
            //LUA_BIND_CONSTRUCTOR(Point, float, Color, PointLight::AttenuationParameters)
            .addFactory([](const Point& p, float rg, float rad, unsigned int nSamples, const Color& c, const AttenuationParameters& at)
            {
				auto ptr = std::shared_ptr<SphereLight>(new SphereLight(p, rg, rad, c, at));
				ptr->setNbSamples(nSamples);
				return ptr; 
			})
            LUA_BIND_PROPERTY(SphereLight, radius, getRadius, setRadius)
            LUA_END_BIND_METHODS
    };
    
    class DirectionalLight : public Light {
    protected:
        Vector direction;
    public:
        DirectionalLight(const Color& c, const Vector& inDirection)
            : Light(Point(0,0,0), c, 0, AttenuationParameters(), 1u)
            , direction(normalize(inDirection))
        {}

        DirectionalLight(const DirectionalLight &) = default;

		virtual ~DirectionalLight() = default;

		void setDirection(const Vector& d) { direction = d; }
        const Vector& getDirection() const { return direction; }

        virtual Vector getLightDirection(const Point& p) const { return direction * -1.0f; }
        virtual bool isLightInRange(const Point& p) const { return true; }

        virtual float ShadePoint(RenderingContext_ptr Context, const Point& point, Color& outColor) const;

        LUA_BEGIN_BIND_METHODS_SUBCLASS_OF(DirectionalLight, Light)
            .addFactory([](const Color& c, const Vector& inDirection)
            {
				return std::shared_ptr<DirectionalLight>(new DirectionalLight(c, inDirection));
			})
            LUA_BIND_PROPERTY(DirectionalLight, direction, getDirection, setDirection)
            LUA_END_BIND_METHODS
    };

    using Light_ptr = std::shared_ptr<Light>;
    using PointLight_ptr = std::shared_ptr<PointLight>;
    using SphereLight_ptr = std::shared_ptr<SphereLight>;
    using DirectionalLight_ptr = std::shared_ptr<DirectionalLight>;
} // end namespace FalconEye

#endif
