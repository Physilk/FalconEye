#pragma once

#include <array>
/*#include <vector>

#include "bbox.h"
#include "ray.h"

using std::array;
using std::vector;

namespace FalconEye {

    class SceneObject;

    class Octree
    {
    private:
        array<Octree*, 8> childrens;
        vector<SceneObject*> objects;
        BBox bbox;

        Octree() = delete;
    public:
        Octree(const BBox& bbox, vector<SceneObject*> objs, size_t maxDepth);
        ~Octree();

        void split(size_t max_depth);
        bool intersect(const Ray&, Hit&);

    };

 
} // end namespace FalconEye*/