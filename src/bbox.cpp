
#include "bbox.h"

namespace FalconEye {
    bool BBox::intersect(const Ray &ray) const {
        float tmin, tmax, tymin, tymax, tzmin, tzmax;
        float divx, divy, divz;

        divx = 1 / ray.direction.x;
        if (divx >= 0) {
            tmin = (min.x - ray.origin.x) * divx;
            tmax = (max.x - ray.origin.x) * divx;
        }
        else {
            tmax = (min.x - ray.origin.x) * divx;
            tmin = (max.x - ray.origin.x) * divx;
        }
        divy = 1 / ray.direction.y;
        if (divy >= 0) {
            tymin = (min.y - ray.origin.y) * divy;
            tymax = (max.y - ray.origin.y) * divy;
        }
        else {
            tymax = (min.y - ray.origin.y) * divy;
            tymin = (max.y - ray.origin.y) * divy;
        }
        if ((tmin > tymax) || (tymin > tmax))
            return false;
        if (tymin > tmin)
            tmin = tymin;
        if (tymax < tmax)
            tmax = tymax;
        divz = 1 / ray.direction.z;
        if (divz >= 0) {
            tzmin = (min.z - ray.origin.z) * divz;
            tzmax = (max.z - ray.origin.z) * divz;
        }
        else {
            tzmax = (min.z - ray.origin.z) * divz;
            tzmin = (max.z - ray.origin.z) * divz;
        }
        if ((tmin > tzmax) || (tzmin > tmax))
            return false;
        /*if(tzmin > tmin)
        tmin = tzmin;
        if(tzmax < tmax)
        tmax = tzmax;*/
        return true;
    }

    bool BBox::isInside(const Point &p) const {
        if (p.x < min.x || p.x > max.x)
            return false;
        if (p.y < min.y || p.y > max.y)
            return false;
        if (p.z< min.z || p.z > max.z)
            return false;
        return true;
    }

    Axis BBox::splitOnLargestFace(Point &min_bis, Point &max_bis) const {
        min_bis = min;
        max_bis = max;

        float l_x = max.x - min.x;
        float l_y = max.y - min.y;
        float l_z = max.z - min.z;

        float half;

        //axe x le plus grand
        if (l_x >= l_y && l_x >= l_z) {
            half = l_x / 2.0f;
            min_bis.x += half;
            max_bis.x -= half;
            return Axis::X_AXIS;
        }
        else if (l_y >= l_x && l_y >= l_z) {
            half = l_y / 2.0f;
            min_bis.y += half;
            max_bis.y -= half;
            return Axis::Y_AXIS;
        }
        else {
            half = l_z / 2.0f;
            min_bis.z += half;
            max_bis.z -= half;
            return Axis::Z_AXIS;
        }

    }

    bool BBox::operator==(const BBox& other) const
    {
        return min == other.min && max == other.max;
    }

}

