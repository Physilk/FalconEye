#include <math.h>

#include "tools.h"

namespace FalconEye {
    namespace Tools {

        bool solveQuadratic(const float &a, const float &b, const float &c, float &x0, float &x1) {
            float discr = b * b - 4 * a * c;
            if (discr < 0) return false;
            else if (discr == 0) x0 = x1 = -0.5 * b / a;
            else {
                float q = (b > 0) ?
                    -0.5 * (b + sqrt(discr)) :
                    -0.5 * (b - sqrt(discr));
                x0 = q / a;
                x1 = c / q;
            }
            if (x0 > x1) std::swap(x0, x1);

            return true;
        }

        // -------------------------------------------------------------

        Vector reflect(const Vector& normal, const Vector& incident) {
            const float cosI = -dot(normal, incident);
            return incident + 2 * cosI * normal;
        }

        // -------------------------------------------------------------

        Vector refract(const Vector& normal, const Vector& incident, float n1, float n2) {
            const float n = n1 / n2;
            const float cosI = dot(normal, incident);
            const float sinT2 = n * n * (1.0f - cosI * cosI);
            if (sinT2 > 1.0f)
                return Vector(0, 0, 0);
            const float cosT = sqrt(1.0f - sinT2);
            return n * incident + (n *cosI - cosT) * normal;
        }

        float fresnelFactor(float fresnel_0, const Vector & normal, const Vector & rayDirection)
        {
            return fresnel_0 + (1 - fresnel_0) * pow((1 - dot(normal, rayDirection)), 5.0f);
        }

    } // end Namespace Tools
} // end namespace FalconEye
