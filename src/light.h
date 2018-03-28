#ifndef LIGHT_HPP
#define LIGHT_HPP

//#include <memory>
#include "gKit/color.h"
#include "gKit/vec.h"

#include "luaObjectInterface.h"

namespace FalconEye {

    class Light {
    protected:
        Color color;

    public:
        Light() { color = Color(1, 1, 1); }
        Light(Color c) : color(c) {}

        Color getColor() const { return color; }
        void setColor(const Color &c) { color = c; }
    };

    class PointLight : public Light {
    public:
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

    public:
        Point position;
        float range;
        AttenuationParameters attenuationParameters;

        PointLight(Point p, float r, Color c, AttenuationParameters ap)
            : Light(c)
            , position(p)
            , range(r)
            , attenuationParameters(ap)
        {}

        PointLight(const PointLight &) = default;

        Color attenuation(const Point &p);

        void setPosition(const Point& p) { position = p; }
        void setRange(float r) { range = r; }

        Point getPosition() const { return position; }
        float getRange() const { return range; }

        LUA_BEGIN_BIND_METHODS(PointLight)
            //LUA_BIND_CONSTRUCTOR(Point, float, Color, PointLight::AttenuationParameters)
            .addFactory([](Point p, float r, Color c, PointLight::AttenuationParameters at) {return std::shared_ptr<PointLight>(new PointLight(p, r, c, at)); })
            LUA_BIND_PROPERTY(PointLight, position, getPosition, setPosition)
            LUA_BIND_PROPERTY(PointLight, range, getRange, setRange)
            LUA_BIND_PROPERTY(PointLight, color, getColor, setColor)
            LUA_END_BIND_METHODS
    };

    using PointLight_ptr = std::shared_ptr<PointLight>;
} // end namespace FalconEye

#endif
