#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <memory>
#include <vector>

#include "gKit/vec.h"

namespace FalconEye {

    namespace Tools {

        bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1);

        Vector reflect(const Vector& normal, const Vector& incident);

        Vector refract(const Vector& normal, const Vector& incident, float n1, float n2);

        float fresnelFactor(float fresnel_0, const Vector& normal, const Vector& rayDirection);

        template<class T>
        std::vector<T*> sharedPtrVecToPtrVec(const std::vector<std::shared_ptr<T>> &v)
        {
            std::vector<T*> return_vec;
            return_vec.reserve(v.size());
            for (auto it = v.begin(); it != v.end(); ++it)
            {
                return_vec.push_back((*it).get());
            }
            return return_vec;
        }
    };
} // end namespace FalconEye


#endif
