#include <algorithm>

#include "light.h"

namespace FalconEye {
    Color PointLight::attenuation(const Point & p)
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
} // end namespace FalconEye
