/*#include "octree.h"
#include "tools.h"
#include "SceneObject.h"
#include <algorithm>

namespace FalconEye {

    Octree::Octree(const BBox& b, vector<SceneObject*> objs, size_t maxDepth)
        : childrens({nullptr})
        , objects(objs)
        , bbox(b)
    {
        if (objs.size() > 0 && maxDepth > 0)
        {
            split(maxDepth);
        }
    }

    Octree::~Octree()
    {
        for (Octree* o : childrens)
        {
            delete o;
        }
    }

    void Octree::split(size_t max_depth)
    {
        BBox new_bbox[8] = { BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
            , BBox(Point(), Point())
        };

        vector<SceneObject*> new_objs[8];

        BBox::divide(bbox, new_bbox);

        for (SceneObject* o : objects)
        {
            for (int i = 0; i < 8; ++i)
            {
                if (o->intersect(new_bbox[i]))
                {
                    new_objs[i].push_back(o);
                }
            }
        }

        for (int i = 0; i < 8; ++i)
        {
            childrens[i] = new Octree(new_bbox[i], new_objs[i], max_depth - 1);
        }

    }

    bool Octree::intersect(const Ray &ray, Hit &hit)
    {
        struct OctreeIntersection
        {
            Octree* o;
            float t;
        };

        if (objects.size() > 0)
        {
            vector<OctreeIntersection> intersections;
            intersections.reserve(10);
            Hit bbox_hit;
            for (Octree* o : childrens)
            {
                if (o != nullptr)
                {
                    if (o->bbox.intersect(ray, bbox_hit))
                    {
                        intersections.push_back(OctreeIntersection({ o, hit.t }));
                    }
                }
            }

            //we are a leaf
            if (intersections.size() == 0)
            {
                bool current_hit = false;
                bool ever_hit = false;
                Hit closest_hit;
                for (size_t i = 0; i < objects.size(); ++i)
                {
                    if ((current_hit = objects[i]->intersect(ray, hit)))
                    {
                        if (!ever_hit || hit.t < closest_hit.t)
                        {
                            closest_hit = hit;
                        }
                        ever_hit = true;
                    }
                }
                if (ever_hit)
                {
                    hit = closest_hit;
                    return true;
                }
            }
            else
            {
                std::sort(intersections.begin(), intersections.end(), [](const OctreeIntersection& a, const OctreeIntersection& b) { return a.t < b.t; });

                bool current_hit = false;
                bool ever_hit = false;
                Hit closest_hit;

                for (auto&& i : intersections)
                {
                    if ((current_hit = i.o->intersect(ray, hit)))
                    {
                        if (!ever_hit || hit.t < closest_hit.t)
                        {
                            closest_hit = hit;
                        }
                        //early cut
                        if (ever_hit && hit.t > closest_hit.t)
                        {
                            hit = closest_hit;
                            return true;
                        }
                        ever_hit = true;
                    }
                }
                if (ever_hit)
                {
                    hit = closest_hit;
                    return true;
                }
            }
            
        }
        return false;
    }

} // end namespace FalconEye

*/