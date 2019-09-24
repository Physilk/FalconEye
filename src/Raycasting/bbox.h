#ifndef BBOX_HPP
#define BBOX_HPP

#include <iostream>
#include <limits>
#include "gKit/vec.h"

#include "ray.h"

namespace FalconEye {
    enum Axis {
        X_AXIS,
        Y_AXIS,
        Z_AXIS
    };

    struct BBox
    {
        Point min;
        Point max;

        BBox() : BBox(InfiniteBox()) {}
        BBox(const Point &p1, const Point &p2) : min(p1),
            max(p2)
        {}

        bool intersect(const Ray &ray) const;
        bool isInside(const Point &p) const;

        Axis splitOnLargestFace(Point &min_bis, Point &max_bis) const;

        bool operator==(const BBox& other) const;

        friend std::ostream & operator<<(std::ostream &os, const BBox &bbox) {
            os << "bbox:\n";
            os << "\tmin: x=" << bbox.min.x << "\n";
            os << "\tmin: y=" << bbox.min.y << "\n";
            os << "\tmin: z=" << bbox.min.z << "\n";
            os << "\tmax: x=" << bbox.max.x << "\n";
            os << "\tmax: y=" << bbox.max.y << "\n";
            os << "\tmax: z=" << bbox.max.z << std::endl;

            return os;
        }

        static const BBox& InfiniteBox()
        {
            static const BBox bbox = BBox(
                Point(-std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max(),
                    -std::numeric_limits<float>::max()
                ),
                Point(-std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max(),
                    std::numeric_limits<float>::max()
                ));
            return bbox;
        }

    };
} // end namespace FalconEye


#endif
