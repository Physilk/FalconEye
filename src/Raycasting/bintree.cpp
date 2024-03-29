#include <vector>

#include "bintree.h"
#include "SceneObject.h"

using std::vector;

namespace FalconEye {

    BBoxBinTreeNode::BBoxBinTreeNode(BBoxBinTreeNode&& other)
        : d(other.d)
        , g(other.g)
        , bbox(std::move(other.bbox))
        , objects(std::move(other.objects))
    {
        other.d = nullptr;
        other.g = nullptr;
    }

    BBoxBinTreeNode::~BBoxBinTreeNode() {
        delete g;
        delete d;
    }

    void BBoxBinTreeNode::split(size_t max_objects_per_node) {
        if (g != nullptr || d != nullptr)
            return;

        if (objects.size() <= max_objects_per_node)
            return;

        Point min_bis, max_bis;
        //bbox.splitOnLargestFace(min_bis, max_bis);
        Axis axis = bbox.splitOnLargestFace(min_bis, max_bis);

        BBox bbox_d(min_bis, bbox.max);
        BBox bbox_g(bbox.min, max_bis);

        Point max_d = bbox.min;
        Point min_g = bbox.max;
        Point min_d = bbox.max;
        Point max_g = bbox.min;

        vector<SceneObject *> d_bis, g_bis;

        SceneObject * current_object;
        for (unsigned int i = 0; i < objects.size(); ++i) {
            current_object = objects[i];
            Point center = current_object->getCenter();
            Point min = current_object->getMin();
            Point max = current_object->getMax();

            if (bbox_d.isInside(center)) {
                d_bis.push_back(current_object);

                if (min_d.x > min.x)
                    min_d.x = min.x;

                if (min_d.y > min.y)
                    min_d.y = min.y;

                if (min_d.z > min.z)
                    min_d.z = min.z;

                if (max_d.x < max.x)
                    max_d.x = max.x;

                if (max_d.y < max.y)
                    max_d.y = max.y;

                if (max_d.z < max.z)
                    max_d.z = max.z;


            }
            else {
                
                g_bis.push_back(current_object);

                if (max_g.x < max.x)
                    max_g.x = max.x;

                if (max_g.y < max.y)
                    max_g.y = max.y;

                if (max_g.z < max.z)
                    max_g.z = max.z;

                if (min_g.x > min.x)
                    min_g.x = min.x;

                if (min_g.y > min.y)
                    min_g.y = min.y;

                if (min_g.z > min.z)
                    min_g.z = min.z;

            }
        }

        bbox_d.min = min_d;
        bbox_d.max = max_d;
        bbox_g.min = min_g;
        bbox_g.max = max_g;

        d = new BBoxBinTreeNode(bbox_d, d_bis);
        g = new BBoxBinTreeNode(bbox_g, g_bis);

        if (g_bis.size() > 0)
            d->split(max_objects_per_node);
        if (d_bis.size() > 0)
            g->split(max_objects_per_node);
    }

    BBoxBinTree::BBoxBinTree(BBox b, const std::vector<SceneObject *> &objects, size_t s)
        : parent()
    {

        if (b == BBox::InfiniteBox())
        {
            std::vector<SceneObject *> newObjectsVector;
            newObjectsVector.reserve(objects.size());

            float limit = std::numeric_limits<float>::max();
            Point boundMin = Point(limit, limit, limit);
            Point boundMax = Point(-limit, -limit, -limit);

            for (SceneObject* o : objects)
            {
                Point min = o->getMin();
                Point max = o->getMax();

                if (BBox(min, max) == BBox::InfiniteBox())
                {
                    infinite_objects.push_back(o);
                }
                else
                {
                    if (boundMax.x < max.x)
                        boundMax.x = max.x;
                    if (boundMax.y < max.y)
                        boundMax.y = max.y;
                    if (boundMax.z < max.z)
                        boundMax.z = max.z;

                    if (boundMin.x > min.x)
                        boundMin.x = min.x;
                    if (boundMin.y > min.y)
                        boundMin.y = min.y;
                    if (boundMin.z > min.z)
                        boundMin.z = min.z;

                    newObjectsVector.push_back(o);
                }
                
            }
            parent.bbox = BBox(boundMin, boundMax);
            parent.objects = newObjectsVector;
            
        }
        else
        {
            parent.bbox = b;
            parent.objects = objects;
        }
        split(s);
    }

    bool BBoxBinTree::intersect(const Ray &ray, Hit &hit) const {

        bool current_hit = false;
        bool ever_hit = false;
        Hit closest_hit;

        
        for (size_t i = 0; i < infinite_objects.size(); ++i) {
            if (infinite_objects[i]->canIntersect(ray) && (current_hit = infinite_objects[i]->intersect(ray, hit))) {
                if (!ever_hit || hit.t < closest_hit.t) {
                    closest_hit = hit;
                }
                ever_hit = true;
            }
        }

        
        Hit other_hit;

        if (parent.intersect(ray, hit))
        {
            if (!ever_hit || hit.t < closest_hit.t) {
                closest_hit = hit;
            }
            ever_hit = true;
        }

        hit = closest_hit;
        return ever_hit;
    }

    bool BBoxBinTreeNode::intersect(const Ray &ray, Hit &hit) const {
        //intersection avec la node?
        if (!bbox.intersect(ray))
            return false;

        bool current_hit = false;
        bool ever_hit = false;
        Hit closest_hit;
        //node terminale
        if (g == nullptr && d == nullptr) {
            for (size_t i = 0; i < objects.size(); ++i) {
                if (objects[i]->canIntersect(ray) && (current_hit = objects[i]->intersect(ray, hit))) {
                    if (!ever_hit || hit.t < closest_hit.t) {
                        closest_hit = hit;
                    }
                    ever_hit = true;
                }
            }

            hit = closest_hit;
            return ever_hit;
        }

        //node non terminale
        if (d->intersect(ray, hit)) {
            closest_hit = hit;
            ever_hit = true;
        }
        if (g->intersect(ray, hit)) {
            if (ever_hit) {
                if (hit.t < closest_hit.t)
                    closest_hit = hit;
            }
            else {
                ever_hit = true;
                closest_hit = hit;
            }
        }

        hit = closest_hit;
        return ever_hit;
    }
}

